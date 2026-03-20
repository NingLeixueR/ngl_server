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
#include "tools/arg_options.h"
#include "tools/tools.h"

#include <string_view>

namespace ngl
{
	namespace robot_manage_cmd
	{
		std::string normalize_ascii_command(std::string avalue)
		{
			tools::transform_tolower(avalue);
			return avalue;
		}

		ENET_PROTOCOL parse_robot_protocol(std::string aprotocol)
		{
			aprotocol = normalize_ascii_command(std::move(aprotocol));
			if (aprotocol == "ws" || aprotocol == "wss")
			{
				return ENET_WS;
			}
			if (aprotocol.empty() || aprotocol == "tcp")
			{
				return ENET_TCP;
			}

			log_warn()->print("robot login protocol [{}] invalid, fallback tcp", aprotocol);
			return ENET_TCP;
		}

		std::string join_with_space(const std::vector<std::string>& avalue)
		{
			std::string ljoined;
			tools::splicing(avalue, " ", ljoined);
			return ljoined;
		}

		std::string join_role_cmd(const std::vector<std::string>& avalue)
		{
			std::string ljoined;
			for (std::size_t i = 0; i < avalue.size(); ++i)
			{
				if (i == 1)
				{
					ljoined.push_back('|');
				}
				else if (i >= 2)
				{
					ljoined.push_back('*');
				}
				ljoined += avalue[i];
			}
			return ljoined;
		}

		pbnet::PROBUFF_NET_CMD make_role_net_cmd(const std::vector<std::string>& avalue)
		{
			pbnet::PROBUFF_NET_CMD lpro;
			std::string lcmd = join_role_cmd(avalue);
			if (!tools::isutf8(lcmd))
			{
				tools::to_utf8(lcmd, lcmd);
			}
			lpro.set_mcmd(lcmd);
			return lpro;
		}

		bool resolve_kcp_target(const _robot& arobot, std::string_view aserver, pbnet::ENUM_KCP& akcpenum, int16_t& aservertid, int16_t& atcount)
		{
			if (aserver == "game")
			{
				akcpenum = pbnet::KCP_ROLE;
				aservertid = nnodeid::tid(arobot.m_gameid);
				atcount = nnodeid::tcount(arobot.m_gameid);
				return true;
			}
			if (aserver == "gateway")
			{
				akcpenum = pbnet::KCP_GATEWAY;
				aservertid = nnodeid::tid(arobot.m_gatewayid);
				atcount = nnodeid::tcount(arobot.m_gatewayid);
				return true;
			}
			return false;
		}

		void log_robot_cmd_overview()
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
	}//namespace robot_manage_cmd

	bool actor_robot_manage::handle(const message<np_robot_pram>& adata)
	{
		auto lrecv = adata.get_data();
		if (lrecv == nullptr || lrecv->m_cmd.empty())
		{
			robot_manage_cmd::log_robot_cmd_overview();
			return true;
		}

		const std::vector<std::string> ltokens = arg_options::split_command_line(lrecv->m_cmd);
		if (ltokens.empty())
		{
			robot_manage_cmd::log_robot_cmd_overview();
			return true;
		}

		std::string lcmd = robot_manage_cmd::normalize_ascii_command(ltokens[0]);
		std::vector<std::string> largs(ltokens.begin() + 1, ltokens.end());
		bool lfrom_help_command = false;

		const auto log_cmd_help = [](std::string_view acmd, const arg_options& aoptions)
			{
				log_info()->print("robot cmd: {}\n{}", acmd, aoptions.help());
			};

		const auto parse_cmd = [&](std::string_view acmd, arg_options& aoptions)
			{
				if (!aoptions.parse(largs) || aoptions.help_requested())
				{
					log_cmd_help(acmd, aoptions);
					return false;
				}
				return true;
			};

		if (lcmd == "help" || lcmd == "--help" || lcmd == "-h")
		{
			if (largs.empty())
			{
				robot_manage_cmd::log_robot_cmd_overview();
				return true;
			}
			lfrom_help_command = true;
			lcmd = robot_manage_cmd::normalize_ascii_command(largs.front());
			largs = { "--help" };
		}
		if (lcmd == "login")
		{
			arg_options loptions("login options");
			loptions.init_flag("help,h", "show help");
			loptions.init_required<std::string>("robot", "robot account");
			loptions.init_options("protocol", "robot transport: tcp or ws", std::string("tcp"));
			loptions.positional("robot", 1);
			loptions.positional("protocol", 1);
			if (!parse_cmd("login", loptions))
			{
				return true;
			}

			std::string lrobot;
			std::string lprotocol;
			loptions.value("robot", lrobot);
			loptions.value("protocol", lprotocol);
			create_robot(lrobot, robot_manage_cmd::parse_robot_protocol(lprotocol));
			return true;
		}
		if (lcmd == "logins")
		{
			arg_options loptions("logins options");
			loptions.init_flag("help,h", "show help");
			loptions.init_required<std::string>("prefix", "robot account prefix");
			loptions.init_required<int>("beg", "first index");
			loptions.init_required<int>("end", "last index");
			loptions.init_options("protocol", "robot transport: tcp or ws", std::string("tcp"));
			loptions.positional("prefix", 1);
			loptions.positional("beg", 1);
			loptions.positional("end", 1);
			loptions.positional("protocol", 1);
			if (!parse_cmd("logins", loptions))
			{
				return true;
			}

			std::string lprefix;
			int lbeg = 0;
			int lend = 0;
			std::string lprotocol;
			loptions.value("prefix", lprefix);
			loptions.value("beg", lbeg);
			loptions.value("end", lend);
			loptions.value("protocol", lprotocol);
			create_robots(lprefix, lbeg, lend, robot_manage_cmd::parse_robot_protocol(lprotocol));
			return true;
		}
		if (lcmd == "c")
		{
			arg_options loptions("c options");
			loptions.init_flag("help,h", "show help");
			loptions.init_required<std::string>("robot", "robot account");
			loptions.init_multitoken<std::vector<std::string>>("arguments", "role command and arguments", true);
			loptions.positional("robot", 1);
			loptions.positional("arguments", -1);
			if (!parse_cmd("c", loptions))
			{
				return true;
			}

			std::string lrobot;
			std::vector<std::string> larguments;
			loptions.value("robot", lrobot);
			loptions.value("arguments", larguments);

			pbnet::PROBUFF_NET_CMD lpro = robot_manage_cmd::make_role_net_cmd(larguments);
			send(get_robot(lrobot), lpro);
			return true;
		}
		if (lcmd == "d")
		{
			arg_options loptions("d options");
			loptions.init_flag("help,h", "show help");
			loptions.init_multitoken<std::vector<std::string>>("arguments", "broadcast command and arguments", true);
			loptions.positional("arguments", -1);
			if (!parse_cmd("d", loptions))
			{
				return true;
			}

			std::vector<std::string> larguments;
			loptions.value("arguments", larguments);

			pbnet::PROBUFF_NET_CMD lpro = robot_manage_cmd::make_role_net_cmd(larguments);
			foreach([&lpro, this](_robot& arobot)
				{
					send(&arobot, lpro);
					return true;
				});
			return true;
		}
		if (lcmd == "kcp")
		{
			arg_options loptions("kcp options");
			loptions.init_flag("help,h", "show help");
			loptions.init_required<int32_t>("kcpnum", "kcp enum");
			loptions.init_required<int16_t>("servertid", "target server tid");
			loptions.init_required<int16_t>("tcount", "target server count");
			loptions.init_required<int64_t>("actorid", "target actor id");
			loptions.init_optional<int64_t>("robotid", "optional robot actor id");
			loptions.positional("kcpnum", 1);
			loptions.positional("servertid", 1);
			loptions.positional("tcount", 1);
			loptions.positional("actorid", 1);
			loptions.positional("robotid", 1);
			if (!parse_cmd("kcp", loptions))
			{
				return true;
			}

			int32_t lkcpenum = 0;
			int16_t lservertid = 0;
			int16_t ltcount = 0;
			int64_t lactorid = 0;
			int64_t lrobotid = 0;
			loptions.value("kcpnum", lkcpenum);
			loptions.value("servertid", lservertid);
			loptions.value("tcount", ltcount);
			loptions.value("actorid", lactorid);
			if (loptions.value("robotid", lrobotid))
			{
				kcp_connect(lrobotid, (pbnet::ENUM_KCP)lkcpenum, lservertid, ltcount, lactorid);
				return true;
			}

			foreach([this, lkcpenum, lservertid, ltcount, lactorid](_robot& arobot)
				{
					if (arobot.m_robot != nullptr)
					{
						kcp_connect(arobot.m_robot->id_guid(), (pbnet::ENUM_KCP)lkcpenum, lservertid, ltcount, lactorid);
					}
					return true;
				});
			return true;
		}
		if (lcmd == "kcp_gettime")
		{
			arg_options loptions("kcp_gettime options");
			loptions.init_flag("help,h", "show help");
			loptions.init_required<std::string>("server", "game or gateway");
			loptions.positional("server", 1);
			if (!parse_cmd("kcp_gettime", loptions))
			{
				return true;
			}

			std::string lserver;
			loptions.value("server", lserver);
			lserver = robot_manage_cmd::normalize_ascii_command(std::move(lserver));

			pbnet::PROBUFF_NET_GET_TIME lpro;
			foreach([&lpro, &lserver](_robot& arobot)
				{
					pbnet::ENUM_KCP lkcpenum;
					int16_t lservertid = 0;
					int16_t ltcount = 0;
					if (!robot_manage_cmd::resolve_kcp_target(arobot, lserver, lkcpenum, lservertid, ltcount))
					{
						log_warn()->print("kcp_gettime server [{}] invalid", lserver);
						return false;
					}
					auto luport = arobot.m_robot->kcp_index(lservertid, ltcount, lkcpenum);
					if (!luport.has_value())
					{
						return false;
					}
					actor::kcp_send(arobot.m_robot->id_guid(), lpro, *luport);
					return true;
				});
			return true;
		}
		if (lcmd == "kcp_protocol")
		{
			arg_options loptions("kcp_protocol options");
			loptions.init_flag("help,h", "show help");
			loptions.init_required<std::string>("server", "game or gateway");
			loptions.init_required<std::string>("pbname", "protobuf message name");
			loptions.init_multitoken<std::vector<std::string>>("json", "json payload", true);
			loptions.positional("server", 1);
			loptions.positional("pbname", 1);
			loptions.positional("json", -1);
			if (!parse_cmd("kcp_protocol", loptions))
			{
				return true;
			}

			std::string lserver;
			std::string lpbname;
			std::vector<std::string> ljson_tokens;
			loptions.value("server", lserver);
			loptions.value("pbname", lpbname);
			loptions.value("json", ljson_tokens);

			lserver = robot_manage_cmd::normalize_ascii_command(std::move(lserver));
			const std::string ljson = robot_manage_cmd::join_with_space(ljson_tokens);
			foreach([&lserver, &lpbname, &ljson](_robot& arobot)
				{
					pbnet::ENUM_KCP lkcpenum;
					int16_t lservertid = 0;
					int16_t ltcount = 0;
					if (!robot_manage_cmd::resolve_kcp_target(arobot, lserver, lkcpenum, lservertid, ltcount))
					{
						log_warn()->print("kcp_protocol server [{}] invalid", lserver);
						return false;
					}

					std::shared_ptr<pack> lpack = actor_base::jsonpack(lpbname, ljson, nguid::moreactor(), arobot.m_actor_roleid);
					if (lpack != nullptr)
					{
						auto luport = arobot.m_robot->kcp_index(lservertid, ltcount, lkcpenum);
						if (luport.has_value())
						{
							actor::kcp_sendpack(arobot.m_robot->id_guid(), lpack, *luport);
						}
					}
					return true;
				});
			return true;
		}
		if (lcmd == "protocol")
		{
			arg_options loptions("protocol options");
			loptions.init_flag("help,h", "show help");
			loptions.init_required<std::string>("pbname", "protobuf message name");
			loptions.init_multitoken<std::vector<std::string>>("json", "json payload", true);
			loptions.positional("pbname", 1);
			loptions.positional("json", -1);
			if (!parse_cmd("protocol", loptions))
			{
				return true;
			}

			std::string lpbname;
			std::vector<std::string> ljson_tokens;
			loptions.value("pbname", lpbname);
			loptions.value("json", ljson_tokens);

			const std::string ljson = robot_manage_cmd::join_with_space(ljson_tokens);
			foreach([&lpbname, &ljson](_robot& arobot)
				{
					std::shared_ptr<pack> lpack = actor_base::jsonpack(lpbname, ljson, nguid::moreactor(), arobot.m_actor_roleid);
					if (lpack != nullptr)
					{
						nnet::instance().send_pack(arobot.m_session, lpack);
					}
					return true;
				});
			return true;
		}
		if (lcmd == "test_thruput")
		{
			arg_options loptions("test_thruput options");
			loptions.init_flag("help,h", "show help");
			loptions.init_required<int32_t>("rounds", "round count");
			loptions.init_required<int32_t>("actorcount", "actor count");
			loptions.init_required<int32_t>("everycount", "messages per actor");
			loptions.positional("rounds", 1);
			loptions.positional("actorcount", 1);
			loptions.positional("everycount", 1);
			if (!parse_cmd("test_thruput", loptions))
			{
				return true;
			}

			int32_t lrounds = 0;
			int32_t lactorcount = 0;
			int32_t leverycount = 0;
			loptions.value("rounds", lrounds);
			loptions.value("actorcount", lactorcount);
			loptions.value("everycount", leverycount);
			for (int i = 0; i < lrounds; ++i)
			{
				test_thruput::instance().add_rounds(lactorcount, leverycount);
			}
			return true;
		}
		if (lcmd == "release_thruput")
		{
			arg_options loptions("release_thruput options");
			loptions.init_flag("help,h", "show help");
			if (!parse_cmd("release_thruput", loptions))
			{
				return true;
			}

			test_thruput::instance().release();
			return true;
		}

		if (lfrom_help_command)
		{
			robot_manage_cmd::log_robot_cmd_overview();
			return true;
		}

		log_error()->print("actor_manage_robot cmd notfind {}", lrecv->m_cmd);
		robot_manage_cmd::log_robot_cmd_overview();
		return true;
	}

	bool actor_robot_manage::handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>& adata)
	{
		auto lrecv = adata.get_data();
		_robot& lrobot = m_maprobot[lrecv->maccount()];
		m_maprobotbyactorid[lrecv->mroleid()] = &lrobot;
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
		auto recv = adata.get_data();
		pbnet::PROBUFF_NET_ROLE_CREATE pro;
		std::string lname = std::format("role_{}", recv->mroleid());
		log_error()->print("鍒涘缓瑙掕壊[{}]", lname);
		pro.set_mname(lname);
		send(get_robot(recv->mroleid()), pro);
		return true;
	}
}//namespace ngl
