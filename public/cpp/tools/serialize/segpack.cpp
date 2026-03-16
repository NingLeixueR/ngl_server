/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
*
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
*
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for serialize.


#include "actor/protocol/protocol.h"
#include "tools/serialize/segpack.h"
#include "tools/localtime.h"
#include "tools/log/nlog.h"

namespace ngl
{
	namespace
	{
		constexpr int32_t segpack_telnet_limit = 10240;
	}

	bool segpack_heartbeat::is_heartbeat(i32_protocolnum aprotocolnum)
	{
		return tprotocol::protocol<pbnet::PROBUFF_NET_HEARTBEAT>() == aprotocolnum;
	}

	void segpack::close(i32_socket aid)
	{
		erase(aid);
	}

	void segpack::erase(i32_socket aid)
	{
		m_data.erase(aid);
		m_rate.erase(aid);
	}

	std::shared_ptr<pack> segpack::remnant_package(i32_socket aid)
	{
		if (auto itor = m_data.find(aid); itor != m_data.end())
		{
			std::shared_ptr<pack> lpack = itor->second;
			m_data.erase(itor);
			return lpack;
		}

		std::shared_ptr<pack> lpack = pack::make_pack(&m_pool, 0);
		if (lpack != nullptr)
		{
			lpack->m_id = aid;
		}
		return lpack;
	}

	segpack::edopush segpack::telnet_cmd(std::shared_ptr<pack>& apack, const char*& ap, int& alen, bool aislanip)
	{
		if (aislanip == false)
		{
			return edopush::e_error;
		}
		if (apack == nullptr || alen < 0 || (alen > 0 && ap == nullptr))
		{
			return edopush::e_error;
		}

		pack_head& lhead = apack->m_head;
		if (apack->m_buff == nullptr)
		{
			if (!apack->malloc(segpack_telnet_limit) || apack->m_buff == nullptr)
			{
				return edopush::e_error;
			}
			memcpy(apack->m_buff, lhead.m_data, lhead.m_wpos);
			apack->m_pos += lhead.m_wpos;
		}
		if (apack->m_pos + alen >= segpack_telnet_limit)
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
		if (apack == nullptr || alen < 0)
		{
			return false;
		}
		if (!aislanip && nconfig.nodetype() != ROBOT && alen >= net_config_recv_buff_maxbyte)
		{
			erase(aid);
			log_error()->print("sockect recv {} len >= SOCKECT_MAX_BUFF_SIZE({})", apack->m_head, (int)net_config_recv_buff_maxbyte);
			return false;
		}
		if (!aislanip && !apack->m_rate_accounted)
		{
			if (!m_rate.add(aid))
			{
				return false;
			}
			apack->m_rate_accounted = true;
		}
		return true;
	}

	bool segpack::is_heartbeat(std::shared_ptr<pack>& apack)
	{
		return apack != nullptr && segpack_heartbeat::is_heartbeat(apack->m_head.get_protocolnumber());
	}

	bool segpack::fill_pack(std::shared_ptr<pack>& apack, const char*& ap, int& alen, int& len)
	{
		if (apack == nullptr)
		{
			return false;
		}
		if (apack->m_buff == nullptr)
		{
			if (!apack->malloc(len))
			{
				return false;
			}
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
		if (ltemp > 0 && ap == nullptr)
		{
			return false;
		}

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
		if (ap == nullptr)
		{
			return edopush::e_error;
		}

		std::shared_ptr<pack> lpack = remnant_package(aid);
		if (lpack == nullptr)
		{
			return edopush::e_error;
		}

		const EPH_HEAD_VAL lval = lpack->m_head.push(ap, alen);
		if (lval == EPH_HEAD_MASK_FAIL)
		{
			return telnet_cmd(lpack, ap, alen, aislanip);
		}
		if (lval == EPH_HEAD_FOLLOW)
		{
			m_data.insert_or_assign(aid, lpack);
			return edopush::e_break;
		}
		if (alen < 0)
		{
			return edopush::e_break;
		}

		int len = lpack->m_head.getvalue(EPH_BYTES);
		if (!check_recv(lpack, aid, len, aislanip))
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
		if (!fill_pack(lpack, ap, alen, len))
		{
			return edopush::e_error;
		}
		if (!lpack->isready())
		{
			m_data.insert_or_assign(aid, lpack);
			return edopush::e_break;
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

	bool segpack::push(i32_socket aid, const char* ap, int alen, bool aislanip)
	{
		if (ap == nullptr && alen == 0)
		{
			erase(aid);
			return true;
		}
		if (alen < 0 || (ap == nullptr && alen > 0))
		{
			return false;
		}

		do
		{
			const edopush levalue = do_push(aid, ap, alen, aislanip);
			if (levalue == edopush::e_error)
			{
				return false;
			}
			if (levalue == edopush::e_break)
			{
				break;
			}
		} while (true);

		return alen <= 0;
	}
}// namespace ngl