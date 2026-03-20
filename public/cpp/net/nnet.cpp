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
// File overview: Implements session-based transport dispatch across tcp and ws.

#include "net/nnet.h"

namespace ngl
{
	ENET_PROTOCOL nnet::protocol(i32_sessionid asession) const
	{
		return net_session::protocol(asession);
	}

	bpool& nnet::pool()
	{
		return ntcp::instance().pool();
	}

	bpool& nnet::pool(ENET_PROTOCOL aprotocol)
	{
		switch (aprotocol)
		{
		case ENET_WS:
			return nws::instance().pool();
		case ENET_TCP:
		default:
			return ntcp::instance().pool();
		}
	}

	bpool& nnet::pool(i32_sessionid asession)
	{
		return pool(protocol(asession));
	}

	void nnet::close(i32_sessionid asession)
	{
		switch (protocol(asession))
		{
		case ENET_TCP:
			ntcp::instance().close(asession);
			return;
		case ENET_WS:
			nws::instance().close(asession);
			return;
		default:
			return;
		}
	}

	void nnet::close_net(i32_sessionid asession)
	{
		switch (protocol(asession))
		{
		case ENET_TCP:
			ntcp::instance().close_net(asession);
			return;
		case ENET_WS:
			nws::instance().close_net(asession);
			return;
		default:
			return;
		}
	}

	bool nnet::send_msg(i32_sessionid asession, const std::string& amsg)
	{
		switch (protocol(asession))
		{
		case ENET_TCP:
			return ntcp::instance().send_msg(asession, amsg);
		case ENET_WS:
			return nws::instance().send_msg(asession, amsg);
		default:
			return false;
		}
	}

	bool nnet::send_pack(i32_sessionid asession, std::shared_ptr<pack>& apack)
	{
		switch (protocol(asession))
		{
		case ENET_TCP:
			return ntcp::instance().send_pack(asession, apack);
		case ENET_WS:
			return nws::instance().send_pack(asession, apack);
		default:
			return false;
		}
	}

	bool nnet::send_pack(i32_sessionid asession, std::shared_ptr<void>& apack)
	{
		switch (protocol(asession))
		{
		case ENET_TCP:
			return ntcp::instance().send_pack(asession, apack);
		case ENET_WS:
			return nws::instance().send_pack(asession, apack);
		default:
			return false;
		}
	}

	bool nnet::send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_sessionid lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return send_pack(lsession, apack);
	}

	bool nnet::send_server(i32_serverid aserverid, std::shared_ptr<void>& apack)
	{
		i32_sessionid lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return send_pack(lsession, apack);
	}

	bool nnet::send(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::shared_ptr<pack>& apack)
	{
		std::map<i32_sessionid, i64_actorid> ltcpmap;
		std::map<i32_sessionid, i64_actorid> lwsmap;
		split_sessions(asession, ltcpmap, lwsmap);

		bool lret = true;
		if (!ltcpmap.empty() && !ntcp::instance().send(ltcpmap, aactorid, apack))
		{
			lret = false;
		}
		if (!lwsmap.empty() && !nws::instance().send(lwsmap, aactorid, apack))
		{
			lret = false;
		}
		return lret;
	}

	bool nnet::send(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack)
	{
		std::set<i32_sessionid> ltcpset;
		std::set<i32_sessionid> lwsset;
		split_sessions(asession, ltcpset, lwsset);

		bool lret = true;
		if (!ltcpset.empty() && !ntcp::instance().send(ltcpset, aactorid, arequestactorid, apack))
		{
			lret = false;
		}
		if (!lwsset.empty() && !nws::instance().send(lwsset, aactorid, arequestactorid, apack))
		{
			lret = false;
		}
		return lret;
	}
}//namespace ngl
