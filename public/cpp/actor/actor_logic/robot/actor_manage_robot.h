#pragma once

#include "actor_robot.h"

namespace ngl
{
	class actor_manage_robot : public actor
	{
		actor_manage_robot() :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = ACTOR_MANAGE_ROBOT, 
						.m_area = tab_self_area,
						.m_id	= nconfig::m_nodeid, 
						.m_manage_dbclient = false
					},
					.m_weight = 0x7fffffff,
				})
		{
		}
	public:
		struct _robot
		{
			i32_sessionid					m_session = -1;
			std::string						m_account = nullptr;
			std::shared_ptr<actor_robot>	m_robot = nullptr;
			i64_actorid						m_actor_roleid = nguid::make();
		};

		std::map<std::string, _robot> m_maprobot;

		friend class actor_instance<actor_manage_robot>;
		static actor_manage_robot& getInstance() 
		{ 
			return actor_instance<actor_manage_robot>::instance();
		}

		std::shared_ptr<actor_robot> create(i16_area aarea, i32_actordataid aroleid)
		{
			return std::dynamic_pointer_cast<actor_robot>(actor_base::create(ENUM_ACTOR::ACTOR_ROBOT, aarea, aroleid, nullptr));
		}

		static void login(const std::string& aaccount, const std::string& apasswold)
		{
			pbnet::PROBUFF_NET_ACOUNT_LOGIN pro;
			pro.set_m_area(tab_self_area);
			pro.set_m_account(aaccount);
			pro.set_m_password(apasswold);
			const tab_servers* tab = ttab_servers::tab();
			assert(tab != nullptr);
			nets::sendbyserver(tab->m_login, pro, nguid::moreactor(), getInstance().id_guid());
		}

		static void nregister();

		bool check_connect(i32_serverid aserverid)const
		{
			return ttab_servers::tab(aserverid) != nullptr && ttab_servers::connect(aserverid) != nullptr;
		}

		void connect(i32_serverid aserverid, const std::function<void(i32_sessionid)>& afun) const
		{
			if (check_connect(aserverid))
			{
				nets::connect(aserverid, afun, true, false);
			}
		}

		bool handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>& adata)
		{
			auto lrecv = adata.get_data();
			_robot& lrobot			= m_maprobot[lrecv->m_account()];
			lrobot.m_robot			= create((i16_area)lrecv->m_area(), nguid::actordataid(lrecv->m_roleid()));
			lrobot.m_account		= lrecv->m_account();
			lrobot.m_actor_roleid	= nguid::make_type(lrobot.m_robot->id_guid(), ACTOR_ROLE);

			connect(lrecv->m_gatewayid(), [lrecv, &lrobot, this](int asession)
				{
					lrobot.m_session = asession;
					lrobot.m_robot->m_session = asession;
					pbnet::PROBUFF_NET_ROLE_LOGIN pro;
					pro.set_m_roleid(lrecv->m_roleid());
					pro.set_m_session(lrecv->m_session());
					pro.set_m_area(lrecv->m_area());
					pro.set_m_iscreate(false);
					pro.set_m_gatewayid(lrecv->m_gatewayid());
					nets::sendbysession(asession, pro, nguid::moreactor(), this->id_guid());
				});
			
			return true;
		}

		static bool parse_command(std::vector<std::string>& aparm)
		{
			auto ldata = std::make_shared<np_robot_pram>();
			ldata->m_parm.swap(aparm);
			i64_actorid lid = ngl::nguid::make(ACTOR_MANAGE_ROBOT, tab_self_area, nconfig::m_nodeid);
			handle_pram lparm = ngl::handle_pram::create<np_robot_pram, false, false>(lid, nguid::moreactor(), ldata);
			actor_manage::getInstance().push_task_id(lid, lparm, false);
			return true;
		}

		using handle_cmd = cmd<actor_manage_robot, std::string, const std::vector<std::string>&>;

		bool handle(const message<np_robot_pram>& adata)
		{
			auto lrecv = adata.get_data();
			std::string lparm1;
			if (lrecv->m_parm.size() > 1)
			{
				lparm1 = lrecv->m_parm[0];
				std::ranges::transform(lparm1, lparm1.begin(), tolower);
			}

			if (handle_cmd::empty())
			{
				handle_cmd::push("logins", [this](const std::vector<std::string>& avec)
					{
						const std::string& lrobotname = avec[1];
						int lbeg = tools::lexical_cast<int>(avec[2].c_str());
						int lend = tools::lexical_cast<int>(avec[3].c_str());
						create_robots(lrobotname, lbeg, lend);
					});
				handle_cmd::push("login", [this](const std::vector<std::string>& avec)
					{
						create_robot(avec[1]);
					});
				// c libo1 1
				handle_cmd::push("c", [this](const std::vector<std::string>& avec)
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
						pro.set_m_cmd(lstream.str());
						send(get_robot(avec[1]), pro);
					});
				// C 1
				handle_cmd::push("d", [this](const std::vector<std::string>& avec)
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
						pro.set_m_cmd(lstream.str());
						foreach([&pro, this](actor_manage_robot::_robot& arobot)
							{
								send(&arobot, pro);
								return true;
							});
					});
				// 进行kcp连接
				handle_cmd::push("x1", [this](const std::vector<std::string>& avec)
					{
						foreach([this](actor_manage_robot::_robot& arobot)
							{
								const tab_servers* tab = ttab_servers::tab();
								const tab_servers* tabgame = ttab_servers::tab("game", tab->m_area, 0);
								if (tabgame == nullptr)
								{
									return true;
								}
								const net_works* lpstruct = ttab_servers::get_nworks(ENET_KCP);
								const net_works* lpstructgame = ttab_servers::nworks(ENET_KCP);
								// 获取本机uip
								ngl::asio_udp_endpoint lendpoint(
									asio::ip::address::from_string(nets::ip(lpstructgame)), lpstructgame->m_port
								);
								i32_session lsession = arobot.m_session;
								//i64_actorid lactorid = id_guid();
								i64_actorid lactorid = arobot.m_robot->id_guid();

								int16_t lkcp = nets::create_kcp();
								arobot.m_robot->m_kcp = lkcp;
								nets::kcp(arobot.m_robot->m_kcp)->sendu_waitrecv(lendpoint, "GetIp", sizeof("GetIp")
									, [this, tabgame, lpstruct, lsession, lactorid, lkcp](char* buff, int len)
									{
										log_error()->print("GetIp Finish : {}", buff);
										ukcp::m_localuip = buff;
										// 获取kcp-session
										pbnet::PROBUFF_NET_KCPSESSION pro;
										pro.set_m_serverid(tabgame->m_id);
										pro.set_m_uip(ukcp::m_localuip);
										pro.set_m_uport(lkcp);
										pro.set_m_conv(ukcp::m_conv);
										nets::sendbysession(lsession, pro, nguid::moreactor(), lactorid);
									});
								return true;
							});
					});
				// 使用kcp连接发送GET_TIME协议
				handle_cmd::push("x2", [this](const std::vector<std::string>& avec)
					{
						pbnet::PROBUFF_NET_GET_TIME pro;
						foreach([&pro, this](actor_manage_robot::_robot& arobot)
							{
								sendkcp(&arobot, pro);
								return true;
							});
					});

				handle_cmd::push("x3", [this](const std::vector<std::string>& avec)
					{
						pbnet::PROBUFF_NET_FRIEND pro;
						foreach([&pro, this](actor_manage_robot::_robot& arobot)
							{
								send(&arobot, pro);
								return true;
							});
					});

				handle_cmd::push("protocol", [this](const std::vector<std::string>& avec)
					{
						foreach([this,&avec](actor_manage_robot::_robot& arobot)
							{
								std::shared_ptr<pack> lpack = actor_base::jsonpack(avec[1], avec[2], nguid::moreactor(), arobot.m_actor_roleid);
								if (lpack != nullptr)
								{
									nets::sendpack(arobot.m_session, lpack);
								}
								return true;
							});
					});
			}

			if (handle_cmd::function(lparm1, lrecv->m_parm) == false)
			{
				log_error()->print("actor_manage_robot cmd notfind {}", lrecv->m_parm);
			}
			return true;
		}
		
		void create_robots(const std::string& arobotname, int abeg, int aend)
		{
			for (int i = abeg; i <= aend; ++i)
			{
				std::string lname(arobotname);
				lname += "";
				lname += tools::lexical_cast<std::string>(i);
				ngl::actor_manage_robot::login(lname, "123456");
			}
		}

		void create_robot(const std::string& arobotname)
		{
			ngl::actor_manage_robot::login(arobotname, "123456");
		}

		void foreach(const std::function<bool(_robot&)>& afun)
		{
			for (std::pair<const std::string, _robot>& item : m_maprobot)
			{
				afun(item.second);
			}
		}

		_robot* get_robot(std::string aacount)
		{
			auto itor = m_maprobot.find(aacount);
			if (itor == m_maprobot.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		template <typename T>
		void send(_robot* arobot, T& adata)
		{
			nets::sendbysession(arobot->m_session, adata, nguid::moreactor(), arobot->m_actor_roleid);
		}

		template <typename T>
		void sendkcp(_robot* arobot, T& adata)
		{
			arobot->m_robot->sendkcp(adata, nguid::moreactor(), arobot->m_robot->m_kcp);
		}		

		bool getdata(_robot* arobot)
		{
			if (arobot == nullptr)
			{
				return false;
			}
			return true;
		}
	};
}//namespace ngl
	
