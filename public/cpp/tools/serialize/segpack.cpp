#include "localtime.h"
#include "protocol.h"
#include "segpack.h"
#include "nlog.h"

namespace ngl
{
	segpack_rate_interval::segpack_rate_interval()
		:m_resetutc((int)localtime::gettime()), m_ratecount(0)
	{}

	void segpack_rate_interval::reset()
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
			m_data.insert(std::make_pair(aid, segpack_rate_interval()));
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

	struct segpack::impl_segpack
	{
		std::map<i32_socket, std::shared_ptr<pack>> m_data; //残包暂时保管
		bpool			m_pool;
		segpack_rate	m_rate;
		segpack*		m_segpack;

		impl_segpack(segpack* asegpack) :
			m_segpack(asegpack)
		{}

		void erase(i32_socket aid)
		{
			m_data.erase(aid);
		}

		enum class edopush
		{
			e_continue,		// 继续循环
			e_break,		// 跳出循环
			e_error,		// 出错
		};

		std::shared_ptr<pack> remnant_package(i32_socket aid)
		{
			std::shared_ptr<pack> lpack = nullptr;
			// # 查看有没有残包
			if (auto itor = m_data.find(aid); itor != m_data.end())
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

		inline edopush telnet_cmd(std::shared_ptr<pack>& apack, const char*& ap, int& alen, bool aislanip)
		{
			if (aislanip == false)
			{
				return edopush::e_error;
			}
			// 支持telnet命令访问，telnet ip port 后记得['CTRL+]']
			// 只支持256个字符的进程命令
			pack_head& lhead = apack->m_head;
			if (apack->m_buff == nullptr)
			{
				apack->malloc(256);
				memcpy(apack->m_buff, lhead.m_data, lhead.m_wpos);
				apack->m_pos += lhead.m_wpos;
			}
			if (apack->m_pos + alen >= 256)
			{
				return edopush::e_error;
			}
			memcpy(&apack->m_buff[apack->m_pos], ap, alen);
			apack->m_pos += alen;
			ap += alen;
			alen -= alen;
			apack->m_buff[apack->m_pos] = '\0';
			protocol::cmd(apack);
			return edopush::e_break;
		}

		inline bool check_recv(std::shared_ptr<pack>& apack, i32_socket aid, int alen, bool aislanip)
		{
			if (alen < 0)
			{
				return false;
			}
			if (aislanip == false && nconfig::node_type() != ROBOT && alen >= net_config_recv_buff_maxbyte)
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

		bool is_heartbeat(std::shared_ptr<pack>& apack)
		{
			//只有包头的包
			//检测是否是心跳包
			if (segpack_heartbeat::is_heartbeat(apack->m_head.get_protocolnumber()))
			{
				return true;
			}
			return false;
		}

		bool fill_pack(std::shared_ptr<pack>& apack, const char*& ap, int& alen, int& len)
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

		inline edopush do_push(i32_socket aid, const char*& ap, int& alen, bool aislanip)
		{
			if (alen <= 0)
			{
				return edopush::e_break;
			}
			std::shared_ptr<pack> lpack = remnant_package(aid);
			if (EPH_HEAD_VAL lval = lpack->m_head.push(ap, alen); lval == EPH_HEAD_MASK_FAIL)
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

				if (localtime::gettime() < lpack->m_head.getvalue(EPH_TIME) + sysconfig::net_timeout())
				{
					protocol::push(lpack);
				}
				else
				{
					log_error()->print("time[{} < {} + {} ]", localtime::gettime(), lpack->m_head.getvalue(EPH_TIME), sysconfig::net_timeout());
				}
				return edopush::e_continue;
			}
			m_data[aid] = lpack;
			return edopush::e_break;
		}

		bool push(i32_socket aid, const char* ap, int alen, bool aislanip)
		{
			if (ap == nullptr && alen == 0)
			{
				erase(aid);
			}

			for (;;)
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
			}
			if (alen > 0)
			{
				return false;
			}
			return true;
		}
	};

	segpack::segpack()
	{
		m_impl_segpack.make_unique(this);
	}

	bool segpack::push(i32_socket aid, const char* ap, int alen, bool aislanip)
	{
		return m_impl_segpack()->push(aid, ap, alen, aislanip);
	}

	void segpack::close(i32_socket aid)
	{
		//有可能缓存残包
		m_impl_segpack()->erase(aid);
	}
}// namespace ngl