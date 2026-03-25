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
// File overview: Implements message handlers for message.

#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "tools/tools.h"

#include <string_view>
#include <charconv>
#include <limits>
#include <vector>
#include <cctype>

namespace ngl
{
	void actor_robot_manage::help()
	{
		log_info()->print(
			"robot cmds:\n"
			"  login <robot> [protocol]\n"
			"  logins <prefix> <beg> <end> [protocol]\n"
			"  c <robot> <cmd> [args...]\n"
			"  d <cmd> [args...]\n"
			"  kcp <kcpnum> <servertid> <tcount> <actorid> [robotid]\n"
			"  kcp_gettime <game|gateway>\n"
			"  kcp_protocol <game|gateway> <pbname> <json...>\n"
			"  protocol <pbname> <json...>\n"
			"  test_thruput <rounds> <actorcount> <everycount>\n"
			"  release_thruput\n"
			"  help [command]"
		);
	}

	bool actor_robot_manage::handle(const message<np_robot_pram>& adata)
	{
		const auto* lrecv = adata.get_data();
		if (lrecv == nullptr || lrecv->m_cmd.empty())
		{
			help();
			return true;
		}
		struct cmd_ctx
		{
			std::string					m_cmd;
			std::vector<std::string>	m_args;
		};
		cmd_ctx lctx;
		std::string lstrcmd(lrecv->m_cmd);
		tools::transform_tolower(lstrcmd);
		if (!tools::splite(lstrcmd.c_str(), " ", lctx.m_cmd, lctx.m_args))
		{
			help();
			return true;
		}

		if (lctx.m_cmd == "help")
		{
			help();
			return true;
		}

		auto lcheck = [](const cmd_ctx& actx, std::size_t amin, std::size_t amax = 9999)->bool{
			return actx.m_args.size() >= amin || actx.m_args.size() < amax;
		};

		if (lctx.m_cmd == "login")
		{// login <robot> [protocol]
			if (!lcheck(lctx, 2, 3))
			{
				return true;
			}

			const std::string& lrobot = lctx.m_args[0];
			const std::string& lprotocol = lctx.m_args[1];
			if (lprotocol == "ws" || lprotocol == "wss")
			{
				create_robot(lrobot, ENET_PROTOCOL::ENET_WS);
			}
			else if(lprotocol == "tcp")
			{
				create_robot(lrobot, ENET_PROTOCOL::ENET_TCP);
			}
			return true;
		}
		if (lctx.m_cmd == "logins")
		{// logins <robot> <beg> <end> [protocol]
			if (!lcheck(lctx, 4))
			{
				return true;
			}

			const std::string& lrobot = lctx.m_args[0];
			const int32_t lbeg = tools::lexical_cast<int32_t>(lctx.m_args[1]);
			const int32_t lend = tools::lexical_cast<int32_t>(lctx.m_args[2]);
			const std::string& lprotocol = lctx.m_args[3];

			if (lprotocol == "ws" || lprotocol == "wss")
			{
				create_robots(lrobot, lbeg, lend, ENET_PROTOCOL::ENET_WS);
			}
			else if (lprotocol == "tcp")
			{
				create_robots(lrobot, lbeg, lend, ENET_PROTOCOL::ENET_TCP);
			}
			return true;
		}
		if (lctx.m_cmd == "c")
		{//c <robot> <cmd> [args...]
			if (!lcheck(lctx, 2))
			{
				return true;
			}

			pbnet::PROBUFF_NET_CMD lpro;
			std::string& lcmd = *lpro.mutable_mcmd();

			const std::string& lrobot = lctx.m_args[0];
			lcmd = std::format("{}|", lctx.m_args[1]);
			std::vector<std::string> lvals(lctx.m_args.begin() + 2, lctx.m_args.end());
			tools::splicing(lvals, "*", lcmd);
			send(get_robot(lrobot), lpro);
			return true;
		}
		if (lctx.m_cmd == "d")
		{// d <cmd> [args...]
			if (!lcheck(lctx, 2))
			{
				return true;
			}

			pbnet::PROBUFF_NET_CMD lpro;
			std::string& lcmd = *lpro.mutable_mcmd();

			lcmd = std::format("{}|", lctx.m_args[0]);
			std::vector<std::string> lvals(lctx.m_args.begin() + 1, lctx.m_args.end());
			tools::splicing(lvals, "*", lcmd);

			foreach([&lpro, this](_robot& arobot)
				{
					send(&arobot, lpro);
				});
			return true;
		}
		if (lctx.m_cmd == "kcp")
		{//kcp <kcpnum> <servertid> <tcount> <actorid> [robotid]
			if (!lcheck(lctx, 4, 6))
			{
				return true;
			}

			const int32_t lkcpenum = tools::lexical_cast<int32_t>(lctx.m_args[0]);
			const int16_t lservertid = tools::lexical_cast<int16_t>(lctx.m_args[1]);
			const int16_t ltcount = tools::lexical_cast<int16_t>(lctx.m_args[2]);
			const int64_t lactorid = tools::lexical_cast<int64_t>(lctx.m_args[3]);

			if (lctx.m_args.size() == 5)
			{
				const int64_t lrobotid = tools::lexical_cast<int64_t>(lctx.m_args[4]);
				kcp_connect(lrobotid, (pbnet::ENUM_KCP)lkcpenum, lservertid, ltcount, lactorid);
				return true;
			}

			foreach([this, lkcpenum, lservertid, ltcount, lactorid](_robot& arobot)
				{
					if (arobot.m_robot != nullptr)
					{
						kcp_connect(arobot.m_robot->id_guid(), (pbnet::ENUM_KCP)lkcpenum, lservertid, ltcount, lactorid);
					}
				});
			return true;
		}
		if (lctx.m_cmd == "kcp_gettime")
		{// kcp_gettime <game|gateway>
			if (!lcheck(lctx, 2))
			{
				return true;
			}

			std::string lserver = lctx.m_args[0];

			pbnet::PROBUFF_NET_GET_TIME lpro;
			foreach([&lpro, &lserver](_robot& arobot)
				{
					std::optional<i16_port> luport;
					if (lserver == "game")
					{
						luport = arobot.m_robot->kcp_index(nnodeid::tid(arobot.m_gameid), nnodeid::tcount(arobot.m_gameid), pbnet::KCP_ROLE);
					}
					if (lserver == "gateway")
					{
						luport = arobot.m_robot->kcp_index(nnodeid::tid(arobot.m_gatewayid), nnodeid::tcount(arobot.m_gatewayid), pbnet::KCP_GATEWAY);
					}
					else
					{
						return;
					}
					if (!luport.has_value())
					{
						return;
					}
					actor::kcp_send(arobot.m_robot->id_guid(), lpro, *luport);
				});
			return true;
		}
		if (lctx.m_cmd == "kcp_protocol")
		{//kcp_protocol <game|gateway> <pbname> <json...>
			if (!lcheck(lctx, 4))
			{
				return true;
			}

			std::string lserver = lctx.m_args[0];
			const std::string& lpbname = lctx.m_args[1];
			const std::string& lpbjson = lctx.m_args[2];
			foreach([&lserver, &lpbname, &lpbjson](_robot& arobot)
				{
					std::optional<i16_port> luport;
					if (lserver == "game")
					{
						luport = arobot.m_robot->kcp_index(nnodeid::tid(arobot.m_gameid), nnodeid::tcount(arobot.m_gameid), pbnet::KCP_ROLE);
					}
					if (lserver == "gateway")
					{
						luport = arobot.m_robot->kcp_index(nnodeid::tid(arobot.m_gatewayid), nnodeid::tcount(arobot.m_gatewayid), pbnet::KCP_GATEWAY);
					}
					else
					{
						return;
					}

					std::shared_ptr<pack> lpack = actor_base::jsonpack(lpbname, lpbjson, nguid::moreactor(), arobot.m_actor_roleid);
					if (lpack != nullptr)
					{
						actor::kcp_sendpack(arobot.m_robot->id_guid(), lpack, *luport);
					}
				});
			return true;
		}
		if (lctx.m_cmd == "protocol")
		{//protocol <pbname> <json...>
			if (!lcheck(lctx, 3))
			{
				return true;
			}

			const std::string& lpbname = lctx.m_args[0];
			const std::string& lpbjson = lctx.m_args[1];
			foreach([&lpbname, &lpbjson](_robot& arobot)
				{
					std::shared_ptr<pack> lpack = actor_base::jsonpack(lpbname, lpbjson, nguid::moreactor(), arobot.m_actor_roleid);
					if (lpack != nullptr)
					{
						nnet::instance().send_pack(arobot.m_session, lpack);
					}
				});
			return true;
		}
		if (lctx.m_cmd == "test_thruput")
		{//test_thruput <rounds> <actorcount> <everycount>
			if (!lcheck(lctx, 3))
			{
				return true;
			}

			const int32_t lrounds = tools::lexical_cast<int32_t>(lctx.m_args[0]);
			const int32_t lactorcount = tools::lexical_cast<int32_t>(lctx.m_args[1]);
			const int32_t leverycount = tools::lexical_cast<int32_t>(lctx.m_args[2]);
			for (int li = 0; li < lrounds; ++li)
			{
				test_thruput::instance().add_rounds(lactorcount, leverycount);
			}
			return true;
		}
		if (lctx.m_cmd == "release_thruput")
		{//release_thruput
			test_thruput::instance().release();
			return true;
		}

		log_error()->print("actor_manage_robot cmd notfind {}", lrecv->m_cmd);
		help();
		return true;
	}

	bool actor_robot_manage::handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>& adata)
	{
		const auto* lrecv = adata.get_data();
		auto lrobot_it = m_maprobot.try_emplace(lrecv->maccount()).first;
		_robot& lrobot = lrobot_it->second;
		m_maprobotbyactorid.insert_or_assign(lrecv->mroleid(), &lrobot);
		lrobot.m_robot = create((i16_area)lrecv->marea(), nguid::actordataid(lrecv->mroleid()));
		lrobot.m_robot->m_robot = &lrobot;
		lrobot.m_account = lrecv->maccount();
		lrobot.m_actor_roleid = nguid::make_type(lrobot.m_robot->id_guid(), ACTOR_ROLE);
		lrobot.m_gameid = lrecv->mgameid();
		lrobot.m_gatewayid = lrecv->mgatewayid();

		log_error()->print(
			"connect game[{}:{}] gateway[{}:{}]"
			, nnodeid::tid(lrobot.m_gameid), nnodeid::tcount(lrobot.m_gameid)
			, nnodeid::tid(lrobot.m_gatewayid), nnodeid::tcount(lrobot.m_gatewayid)
		);

		connect(lrecv->mgatewayid(), lrobot.m_protocol, [lrecv, &lrobot, this](int asession)
			{
				lrobot.m_session = asession;
				lrobot.m_robot->m_session = asession;
				pbnet::PROBUFF_NET_ROLE_LOGIN pro;
				pro.set_mroleid(lrecv->mroleid());
				pro.set_msession(lrecv->msession());
				pro.set_marea(lrecv->marea());
				pro.set_mgatewayid(lrecv->mgatewayid());
				nnet::instance().send(asession, pro, nguid::moreactor(), this->id_guid());
			});

		return true;
	}

	bool actor_robot_manage::handle(const message<pbnet::PROBUFF_NET_ROLE_NOT_CREATE>& adata)
	{
		const auto* lrecv = adata.get_data();
		pbnet::PROBUFF_NET_ROLE_CREATE pro;
		std::string lname = std::format("role_{}", lrecv->mroleid());
		log_error()->print("PROBUFF_NET_ROLE_NOT_CREATE[{}]", lname);
		pro.set_mname(lname);
		send(get_robot(lrecv->mroleid()), pro);
		return true;
	}
}//namespace ngl
