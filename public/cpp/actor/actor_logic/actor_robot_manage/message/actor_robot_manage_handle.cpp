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
#include "actor_robot_manage.h"
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
					for (int i = 2; i < avec.size(); ++i)
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
					pro.set_mcmd(lstream.str());
					send(get_robot(avec[1]), pro);
				};

			// C 1
			handle_cmd::add("d") = [this](const std::vector<std::string>& avec)
				{
					pbnet::PROBUFF_NET_CMD pro;
					std::stringstream lstream;
					for (int i = 1; i < avec.size(); ++i)
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
					pro.set_mcmd(lstream.str());
					foreach([&pro, this](_robot& arobot)
						{
							send(&arobot, pro);
							return true;
						});
				};

			// 进行kcp连接
			handle_cmd::add("kcp") = [this](const std::vector<std::string>& avec)
				{
					foreach([this, &avec](_robot& arobot)
						{
							pbnet::ENUM_KCP lkcpenum = (pbnet::ENUM_KCP)tools::lexical_cast<int32_t>(avec[1]);
							int16_t lservertid = tools::lexical_cast<int16_t>(avec[2]);
							int16_t ltcount = tools::lexical_cast<int16_t>(avec[3]);
							int64_t lactorid = tools::lexical_cast<int64_t>(avec[4]);
							kcp_connect(arobot.m_robot->id_guid(), lkcpenum, lservertid, ltcount, lactorid);
							return true;
						});
				};

			// 使用kcp连接发送GET_TIME协议
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
							actor::kcp_send(arobot.m_robot->id_guid(), pro, arobot.m_robot->kcp_index(lservertid, ltcount, lkcpenum));
							return true;
						});
				};

			// kcp json 发送消息
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
								actor::kcp_sendpack(arobot.m_robot->id_guid(), lpack, arobot.m_robot->kcp_index(lservertid, ltcount, lkcpenum));
							}
							return true;
						});
				};

			// tcp json 发送消息
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
				nets::send(asession, pro, nguid::moreactor(), this->id_guid());
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