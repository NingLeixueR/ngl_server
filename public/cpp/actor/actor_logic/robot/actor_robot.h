#pragma once

#include "actor_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_robot : public actor
	{
		// ----- Data Begin -----
		pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE	m_data;
		std::string								m_kcpsessionmd5;
	public:
		i32_session								m_session;
		int16_t									m_kcp;
		// ----- Data End   -----
	public:
		actor_robot(i16_area aarea, i32_actordataid arobotid, void*);

		virtual void init(){}

		virtual ~actor_robot() {}

		static void nregister();

		virtual const char* kcpsessionmd5();

		bool handle(message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_ERROR_RESPONSE>& adata);
		bool handle(message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata);
	};

	class actor_manage_robot : public actor
	{
		actor_manage_robot() :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = ACTOR_MANAGE_ROBOT, 
						.m_area = ttab_servers::tab()->m_area,
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
			i32_sessionid	m_session;
			std::string		m_account;
			actor_robot*	m_robot;
			i64_actorid		m_actor_roleid;
			_robot() :
				m_session(-1),
				m_robot(nullptr),
				m_actor_roleid(nguid::moreactor())
			{}
		};

		std::map<std::string, _robot> m_maprobot;

		friend class actor_instance<actor_manage_robot>;
		static actor_manage_robot& getInstance() 
		{ 
			return actor_instance<actor_manage_robot>::instance();
		}

		actor_robot* create(i16_area aarea, i32_actordataid aroleid)
		{
			return (actor_robot*)actor_base::create(ENUM_ACTOR::ACTOR_ROBOT, aroleid, nullptr);
		}

		static void login(const std::string& aaccount, const std::string& apasswold)
		{
			pbnet::PROBUFF_NET_ACOUNT_LOGIN pro;
			pro.set_m_area(ttab_servers::tab()->m_area);
			pro.set_m_account(aaccount);
			pro.set_m_password(apasswold);
			tab_servers* tab = ttab_servers::tab();

			nets::sendbyserver(tab->m_login, pro, nguid::moreactor(), getInstance().id_guid());
		}

		virtual ~actor_manage_robot() {}

		static void nregister();

		void connect(i32_serverid aserverid, const std::function<void(i32_sessionid)>& afun)
		{
			tab_servers* tab = ttab_servers::tab(aserverid);
			net_works const* lnet = ttab_servers::connect(aserverid);
			assert(lnet != nullptr);
			nets::connect(aserverid, afun, true, false);
		}

		bool handle(message<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>& adata)
		{
			auto lrecv = adata.m_data;
			_robot& lrobot			= m_maprobot[lrecv->m_account()];
			lrobot.m_robot			= create(lrecv->m_area(), nguid::actordataid(lrecv->m_roleid()));
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
	public:

		static bool parse_command(std::vector<std::string>& aparm)
		{
			std::shared_ptr<np_robot_pram> ldata(new np_robot_pram());
			ldata->m_parm.swap(aparm);
			i64_actorid lid = ngl::nguid::make(ACTOR_MANAGE_ROBOT, tab_self_area, nconfig::m_nodeid);
			handle_pram lparm;
			ngl::handle_pram::create<np_robot_pram, false, false>(lparm, lid, nguid::moreactor(), ldata);
			actor_manage::getInstance().push_task_id(lid, lparm, false);
			return true;
		}

		using handle_cmd = cmd<actor_manage_robot, std::string, std::vector<std::string>&>;

		bool handle(message<np_robot_pram>& adata)
		{
			auto lrecv = adata.m_data;
			if (lrecv->m_parm.size() > 1)
			{
				std::transform(lrecv->m_parm[1].begin(), lrecv->m_parm[1].end(), lrecv->m_parm[1].begin(), tolower);
			}

			if (handle_cmd::empty())
			{
				handle_cmd::push("logins", [this](std::vector<std::string>& avec)
					{
						create_robots(avec[1], tools::lexical_cast<int>(avec[2]), tools::lexical_cast<int>(avec[3]));
					});
				handle_cmd::push("login", [this](std::vector<std::string>& avec)
					{
						create_robot(avec[1]);
					});
				// c libo1 1
				handle_cmd::push("c", [this](std::vector<std::string>& avec)
					{
						pbnet::PROBUFF_NET_CMD pro;
						std::string lstr;
						for (int i = 2; i < avec.size(); ++i)
						{
							if (i == 3)
								lstr += '|';
							else if (i >= 4)
								lstr += '*';
							lstr += avec[i];
						}
						pro.set_m_cmd(lstr);
						send(get_robot(avec[1]), pro);
					});
				// C 1
				handle_cmd::push("d", [this](std::vector<std::string>& avec)
					{
						pbnet::PROBUFF_NET_CMD pro;
						std::string lstr;
						for (int i = 1; i < avec.size(); ++i)
						{
							if (i == 2)
								lstr += '|';
							else if (i >= 3)
								lstr += '*';
							lstr += avec[i];
						}
						pro.set_m_cmd(lstr);
						foreach([&pro, this](actor_manage_robot::_robot& arobot)
							{
								send(&arobot, pro);
								return true;
							});
					});
				// 进行kcp连接
				handle_cmd::push("x1", [this](std::vector<std::string>& avec)
					{
						foreach([this](actor_manage_robot::_robot& arobot)
							{
								tab_servers* tab = ttab_servers::tab();
								tab_servers* tabgame = ttab_servers::tab("game", tab->m_area, 1);
								net_works const* lpstruct = ttab_servers::get_nworks(ENET_KCP);
								net_works const* lpstructgame = ttab_servers::get_nworks("game", tab->m_area, 1, ENET_KCP);
								// 获取本机uip
								ngl::asio_udp_endpoint lendpoint(asio::ip::address::from_string(nets::ip(lpstructgame)), lpstructgame->m_port);
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
				handle_cmd::push("x2", [this](std::vector<std::string>& avec)
					{
						pbnet::PROBUFF_NET_GET_TIME pro;
						foreach([&pro, this](actor_manage_robot::_robot& arobot)
							{
								sendkcp(&arobot, pro);
								return true;
							});
					});
			}

			if (handle_cmd::function(lrecv->m_parm[0], lrecv->m_parm) == false)
			{
				log_error()->print("actor_manage_robot cmd notfind {}", lrecv->m_parm);
			}

			return true;
		}
		
		void create_robots(std::string& arobotname, int abeg, int aend)
		{
			for (int i = abeg; i <= aend; ++i)
			{
				std::string lname(arobotname);
				lname += "";
				lname += tools::lexical_cast<std::string>(i);
				ngl::actor_manage_robot::login(lname, "123456");
			}
		}

		void create_robot(std::string& arobotname)
		{
			ngl::actor_manage_robot::login(arobotname, "123456");
		}

		void foreach(const std::function<bool(_robot&)>& afun)
		{
			for (auto& item : m_maprobot)
				afun(item.second);
		}

		_robot* get_robot(std::string aacount)
		{
			auto itor = m_maprobot.find(aacount);
			if (itor == m_maprobot.end())
				return nullptr;
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
				return false;
			return true;
		}
	};
}//namespace ngl
	
