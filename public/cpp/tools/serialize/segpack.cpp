/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "localtime.h"
#include "protocol.h"
#include "segpack.h"
#include "nlog.h"

namespace ngl
{
	segpack_rate::interval::interval() :
		m_resetutc((int)localtime::gettime()), m_ratecount(0)
	{}

	void segpack_rate::interval::reset()
	{
		m_resetutc = (int)localtime::gettime();
		m_ratecount = 0;
	}

	bool segpack_rate::add(i32_socket aid)
	{
		int lnow = (int)localtime::gettime();
		auto itor = m_data.find(aid);
		if (itor == m_data.end())
		{
			m_data.insert(std::make_pair(aid, interval()));
			return true;
		}
		if (itor->second.m_resetutc == 0)
		{
			itor->second.reset();
			return true;
		}
		if (lnow >= itor->second.m_resetutc + sysconfig::rate_interval())
		{
			itor->second.reset();
			return true;
		}
		++itor->second.m_ratecount;
		if (sysconfig::rate_count() < itor->second.m_ratecount)
		{
			return false;
		}
		return true;
	}

	bool segpack_heartbeat::is_heartbeat(i32_protocolnum aprotocolnum)
	{		
		return tprotocol::protocol<pbnet::PROBUFF_NET_HEARTBEAT>() == aprotocolnum;
	}

	void segpack::close(i32_socket aid)
	{
		//有可能缓存残包
		erase(aid);
	}

	void segpack::erase(i32_socket aid)
	{
		m_data.erase(aid);
	}

	std::shared_ptr<pack> segpack::remnant_package(i32_socket aid)
	{
		std::shared_ptr<pack> lpack = nullptr;
		// # 查看有没有残包
		auto itor = m_data.find(aid);
		if (itor != m_data.end())
		{
			lpack = itor->second;
			m_data.erase(itor);
		}
		else
		{
			lpack = pack::make_pack(&m_pool, 0);
			lpack->m_id = aid;
			lpack->m_segpack = m_segpack;
		}
		return lpack;
	}

	segpack::edopush segpack::telnet_cmd(std::shared_ptr<pack>& apack, const char*& ap, int& alen, bool aislanip)
	{
		if (aislanip == false)
		{
			return edopush::e_error;
		}
		// 支持telnet命令访问，telnet ip port 后记得['CTRL+]']
		// 只支持10240个字符的进程命令
		pack_head& lhead = apack->m_head;
		if (apack->m_buff == nullptr)
		{
			apack->malloc(10240);
			memcpy(apack->m_buff, lhead.m_data, lhead.m_wpos);
			apack->m_pos += lhead.m_wpos;
		}
		if (apack->m_pos + alen >= 10240)
		{
			return edopush::e_error;
		}
		memcpy(&apack->m_buff[apack->m_pos], ap, alen);
		apack->m_pos += alen;
		ap += alen;
		alen = 0;
		apack->m_buff[apack->m_pos] = '\0';
		protocol::telnet_cmd(apack);
		return edopush::e_break;
	}

	bool segpack::check_recv(std::shared_ptr<pack>& apack, i32_socket aid, int alen, bool aislanip)
	{
		if (alen < 0)
		{
			return false;
		}
		if (aislanip == false && nconfig.nodetype() != ROBOT && alen >= net_config_recv_buff_maxbyte)
		{
			m_data.erase(aid);
			log_error()->print("sockect recv {} len >= SOCKECT_MAX_BUFF_SIZE({})", apack->m_head, (int)net_config_recv_buff_maxbyte);
			return false;
		}
		if (!aislanip && !m_rate.add(aid))
		{
			return false;
		}
		return true;
	}

	bool segpack::is_heartbeat(std::shared_ptr<pack>& apack)
	{
		//只有包头的包,检测是否是心跳包
		if (segpack_heartbeat::is_heartbeat(apack->m_head.get_protocolnumber()))
		{
			return true;
		}
		return false;
	}

	bool segpack::fill_pack(std::shared_ptr<pack>& apack, const char*& ap, int& alen, int& len)
	{
		if (apack->m_buff == nullptr)
		{
			apack->malloc(len);
		}
		if (len != 0 && apack->m_buff == nullptr)
		{
			return false;
		}
		int ltemp = len - apack->m_pos;
		if (ltemp < 0)
		{
			return false;
		}
		ltemp = ltemp > alen ? alen : ltemp;
		memcpy(&apack->m_buff[apack->m_pos], ap, ltemp);
		apack->m_pos += ltemp;
		alen -= ltemp;
		ap += ltemp;
		return true;
	}

	segpack::edopush segpack::do_push(i32_socket aid, const char*& ap, int& alen, bool aislanip)
	{
		if (alen <= 0)
		{
			return edopush::e_break;
		}
		std::shared_ptr<pack> lpack = remnant_package(aid);
		EPH_HEAD_VAL lval = lpack->m_head.push(ap, alen);
		if (lval == EPH_HEAD_MASK_FAIL)
		{
			return telnet_cmd(lpack, ap, alen, aislanip);
		}
		else
		{
			if (lval == EPH_HEAD_MASK_SUCCESS || lval == EPH_HEAD_FOLLOW)
			{
				m_data.try_emplace(aid, lpack);
				return edopush::e_break;
			}
		}

		if (alen < 0)
		{
			return edopush::e_break;
		}

		int len = lpack->m_head.getvalue(EPH_BYTES);
		if (check_recv(lpack, aid, len, aislanip) == false)
		{
			return edopush::e_error;
		}

		if (len == 0)
		{
			if (is_heartbeat(lpack))
			{
				return edopush::e_break;
			}
			protocol::push(lpack);
			return edopush::e_continue;
		}

		if (fill_pack(lpack, ap, alen, len) == false)
		{
			return edopush::e_error;
		}

		if (lpack->isready())
		{
			if (!aislanip && !m_rate.add(aid))
			{
				return edopush::e_error;
			}

			if (localtime::getsystime() < lpack->m_head.getvalue(EPH_TIME) + sysconfig::net_timeout())
			{
				protocol::push(lpack);
			}
			else
			{
				log_error()->print("segpack time[{} < {} + {} ]", localtime::getsystime(), lpack->m_head.getvalue(EPH_TIME), sysconfig::net_timeout());
			}
			return edopush::e_continue;
		}
		m_data[aid] = lpack;
		return edopush::e_break;
	}

	bool segpack::push(i32_socket aid, const char* ap, int alen, bool aislanip)
	{
		if (ap == nullptr && alen == 0)
		{
			erase(aid);
		}

		do
		{
			edopush levalue = do_push(aid, ap, alen, aislanip);
			if (levalue == edopush::e_error)
			{
				return false;
			}
			if (levalue == edopush::e_break)
			{
				break;
			}
		} while (true);

		if (alen > 0)
		{
			return false;
		}
		return true;
	}
}// namespace ngl