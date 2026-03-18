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
namespace ngl
{
	bool actor_robot_manage::handle(const message<np_robot_pram>& adata)
	{
		auto lrecv = adata.get_data();
		std::string lparm1;
		if (lrecv->m_parm.size() >= 1)
		{
			lparm1 = lrecv->m_parm[0];
			std::ranges::transform(lparm1, lparm1.begin(), tolower);
		}

		if (handle_cmd::empty())
		{
			handle_cmd::add("logins") = [this](const std::vector<std::string>& avec)
				{
					const std::string& lrobotname = avec[1];
					int lbeg = tools::lexical_cast<int>(avec[2].c_str());
					int lend = tools::lexical_cast<int>(avec[3].c_str());
					create_robots(lrobotname, lbeg, lend);
				};

			handle_cmd::add("login") = [this](const std::vector<std::string>& avec)
				{
					create_robot(avec[1]);
				};

			// c libo1 1
			handle_cmd::add("c") = [this](const std::vector<std::string>& avec)
				{
					pbnet::PROBUFF_NET_CMD pro;
					std::stringstream lstream;
					for (std::size_t i = 2; i < avec.size(); ++i)
					{
						if (i == 3)
						{
							lstream << '|';
						}
						else if (i >= 4)
						{
							lstream << '*';
						}
						lstream << avec[i];
					}

					std::string lmcmd = lstream.str();
					if (!tools::isutf8(lmcmd))
					{
						tools::to_utf8(lmcmd, lmcmd);
					}
					pro.set_mcmd(lmcmd);
					send(get_robot(avec[1]), pro);
				};

			// C 1
			handle_cmd::add("d") = [this](const std::vector<std::string>& avec)
				{
					pbnet::PROBUFF_NET_CMD pro;
					std::stringstream lstream;
					for (std::size_t i = 1; i < avec.size(); ++i)
					{
						if (i == 2)
						{
							lstream << '|';
						}
						else if (i >= 3)
						{
							lstream << '*';
						}
						lstream << avec[i];
					}
					std::string lmcmd = lstream.str();
					if (!tools::isutf8(lmcmd))
					{
						tools::to_utf8(lmcmd, lmcmd);
					}
					pro.set_mcmd(lmcmd);
					foreach([&pro, this](_robot& arobot)
						{
							send(&arobot, pro);
							return true;
						});
				};

			// Kcpconnection
			handle_cmd::add("kcp") = [this](const std::vector<std::string>& avec)
				{
					pbnet::ENUM_KCP lkcpenum = (pbnet::ENUM_KCP)tools::lexical_cast<int32_t>(avec[1]);
					int16_t lservertid = tools::lexical_cast<int16_t>(avec[2]);
					int16_t ltcount = tools::lexical_cast<int16_t>(avec[3]);
					int64_t lactorid = tools::lexical_cast<int64_t>(avec[4]);
					if (avec.size() >= 6)
					{
						i64_actorid lrobotid = tools::lexical_cast<int64_t>(avec[5]);
						kcp_connect(lrobotid, lkcpenum, lservertid, ltcount, lactorid);
						return;
					}
					foreach([this, lkcpenum, lservertid, ltcount, lactorid](_robot& arobot)
						{
							kcp_connect(arobot.m_robot->id_guid(), lkcpenum, lservertid, ltcount, lactorid);
							return true;
						});
				};

			// KcpconnectionsendGET_TIMEprotocol
			handle_cmd::add("kcp_gettime") = [this](const std::vector<std::string>& avec)
				{
					pbnet::PROBUFF_NET_GET_TIME pro;
					foreach([&pro, this, &avec](_robot& arobot)
						{
							std::string lsername = avec[1];
							pbnet::ENUM_KCP lkcpenum;
							int16_t lservertid = 0;
							int16_t ltcount = 0;
							if (lsername == "game")
							{
								lkcpenum = pbnet::KCP_ROLE;
								lservertid = nnodeid::tid(arobot.m_gameid);
								ltcount = nnodeid::tcount(arobot.m_gameid);
							}
							else if (lsername == "gateway")
							{
								lkcpenum = pbnet::KCP_GATEWAY;
								lservertid = nnodeid::tid(arobot.m_gatewayid);
								ltcount = nnodeid::tcount(arobot.m_gatewayid);
							}
							else
							{
								return false;
							}
							auto luport = arobot.m_robot->kcp_index(lservertid, ltcount, lkcpenum);
							if (!luport.has_value())
							{
								return false;
							}
							actor::kcp_send(arobot.m_robot->id_guid(), pro, *luport);
							return true;
						});
				};

			// kcp json sendmessage
			handle_cmd::add("kcp_protocol") = [this](const std::vector<std::string>& avec)
				{
					foreach([this, &avec](_robot& arobot)
						{
							std::string lsername = avec[1];
							pbnet::ENUM_KCP lkcpenum;
							int16_t lservertid = 0;
							int16_t ltcount = 0;
							if (lsername == "game")
							{
								lkcpenum = pbnet::KCP_ROLE;
								lservertid = nnodeid::tid(arobot.m_gameid);
								ltcount = nnodeid::tcount(arobot.m_gameid);
							}
							else if (lsername == "gateway")
							{
								lkcpenum = pbnet::KCP_GATEWAY;
								lservertid = nnodeid::tid(arobot.m_gatewayid);
								ltcount = nnodeid::tcount(arobot.m_gatewayid);
							}
							else
							{
								return false;
							}
							std::shared_ptr<pack> lpack = actor_base::jsonpack(avec[2], avec[3], nguid::moreactor(), arobot.m_actor_roleid);
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
				};

			// tcp json sendmessage
			handle_cmd::add("protocol") = [this](const std::vector<std::string>& avec)
				{
					foreach([this, &avec](_robot& arobot)
						{
							std::shared_ptr<pack> lpack = actor_base::jsonpack(avec[1], avec[2], nguid::moreactor(), arobot.m_actor_roleid);
							if (lpack != nullptr)
							{
								actor::send_pack(arobot.m_session, lpack);
							}
							return true;
						});
				};

			handle_cmd::add("test_thruput") = [this](const std::vector<std::string>& avec)
				{
					int32_t lrounds = tools::lexical_cast<int32_t>(avec[1]);
					int32_t lactorcount = tools::lexical_cast<int32_t>(avec[2]);
					int32_t leverycount = tools::lexical_cast<int32_t>(avec[3]);
					for (int i = 0; i < lrounds; ++i)
					{
						test_thruput::instance().add_rounds(lactorcount, leverycount);
					}
				};

			handle_cmd::add("release_thruput") = [this](const std::vector<std::string>& avec)
				{
					test_thruput::instance().release();
				};
		}

		if (handle_cmd::function(lparm1, lrecv->m_parm) == false)
		{
			log_error()->print("actor_manage_robot cmd notfind {}", lrecv->m_parm);
		}
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

		connect(lrecv->mgatewayid(), [lrecv, &lrobot, this](int asession)
			{
				lrobot.m_session = asession;
				lrobot.m_robot->m_session = asession;
				pbnet::PROBUFF_NET_ROLE_LOGIN pro;
				pro.set_mroleid(lrecv->mroleid());
				pro.set_msession(lrecv->msession());
				pro.set_marea(lrecv->marea());
				pro.set_mgatewayid(lrecv->mgatewayid());
				ntcp::instance().send(asession, pro, nguid::moreactor(), this->id_guid());
			});

		return true;
	}
	bool actor_robot_manage::handle(const message<pbnet::PROBUFF_NET_ROLE_NOT_CREATE>& adata)
	{
		auto recv = adata.get_data();
		pbnet::PROBUFF_NET_ROLE_CREATE pro;
		std::string lname = std::format("role_{}", recv->mroleid());
		log_error()->print("创建角色[{}]", lname);
		pro.set_mname(lname);
		send(get_robot(recv->mroleid()), pro);
		return true;
	}
}//namespace ngl
