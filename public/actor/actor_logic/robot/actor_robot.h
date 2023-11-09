#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "net.pb.h"

#include <boost/shared_ptr.hpp>

namespace ngl
{
	class actor_robot : public actor
	{
		// ----- Data Begin -----
		pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE m_data;
		// ----- Data End   -----
	public:
		actor_robot(i16_area aarea, i32_actordataid arobotid, void*) :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = ACTOR_ROBOT,
						.m_area = aarea, 
						.m_id = arobotid, 
						.m_manage_dbclient = true
					},
					.m_weight = 0x7fffffff,
				})
		{
			assert(aarea == ttab_servers::tab()->m_area);
		}

		virtual void init()
		{
		}

		virtual ~actor_robot() {}

		static void actor_register();

		enum { ACTOR_TYPE = ACTOR_ROBOT};

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata)
		{
			Try
			{
				LogLocalError("[LOGIC_ROLE_SYNC:%:%]", adata.m_role().m_base().m_name(), adata.m_role().m_base().m_lv());
				m_data = adata;
			}Catch;			
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata)
		{
			char lbuff[1024] = { 0 };
			ngl::localtime::time2str(lbuff, 1024, adata.m_utc(), "%y/%m/%d %H:%M:%S");
			//LogLocalError("[%][%]", m_data.m_role().m_base().m_name(), lbuff);
			std::cout << m_data.m_role().m_base().m_name() << ":" << lbuff << std::endl;
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_CHAT_RESPONSE& adata)
		{
			if (adata.m_type() == pbnet::get_chat_list)
			{
				char lbuff[1024] = { 0 };
				for (auto& item : adata.m_chatlist())
				{
					ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
					
					//LogLocalError("[%][%][%] %", lbuff, 
					//	actor_guid::area(item.m_roleid()), item.m_rolename(), item.m_content());

					std::cout << actor_guid::area(item.m_roleid()) << ":" << item.m_rolename() << ":" << item.m_content() << std::endl;
				}
			}
			else if (adata.m_type() == pbnet::chat_speak)
			{
				//LogLocalError("%", (adata.m_stat() ? "[发言成功]" : "[发言失败] "));
				std::cout << (adata.m_stat() ? "[发言成功]" : "[发言失败] ") << std::endl;
			}
			else if (adata.m_type() == pbnet::updata_speck)
			{
				char lbuff[1024] = { 0 };
				for (auto& item : adata.m_chatlist())
				{
					ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
					//LogLocalError("[%][%][%] %", lbuff,
					//	actor_guid::area(item.m_roleid()), item.m_rolename(), item.m_content());
					std::cout << actor_guid::area(item.m_roleid()) << ":" << item.m_rolename() << ":" << item.m_content() << std::endl;
				}
			}
			return true;
		}
		
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata)
		{
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_GET_NOTICE_RESPONSE& adata)
		{
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata)
		{
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata)
		{
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata)
		{
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata)
		{
			return true;
		}

		/*LOGIC_ROLE_SYNC& get()
		{
			return m_data;
		}*/
	private:
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
						.m_id = nconfig::m_nodeid, 
						.m_manage_dbclient = false
					},
					.m_weight = 0x7fffffff,
				})
		{
		}
	public:
		struct _robot
		{
			i32_sessionid m_session;
			std::string m_account;
			actor_robot* m_robot;
			i64_actorid m_actor_roleid;
			_robot() :
				m_session(-1),
				m_robot(nullptr),
				m_actor_roleid(actor_guid::moreactor())
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

			nserver->sendtoserver(tab->m_login, pro, actor_guid::moreactor(), getInstance().id_guid());
		}

		virtual ~actor_manage_robot() {}

		static void actor_register();

		enum { ACTOR_TYPE = ACTOR_MANAGE_ROBOT};

		void connect(i32_serverid aserverid, const std::function<void(i32_sessionid)>& afun)
		{
			tab_servers* tab = ttab_servers::tab(aserverid);
			nserver->connect(aserverid, tab->m_ip, tab->m_port, afun, true, false);
		}


		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
		{
			std::cout << "#############["<< adata.m_account() <<"]登录成功#############" << std::endl;
			_robot& lrobot = m_maprobot[adata.m_account()];
			lrobot.m_robot = create(adata.m_area(), actor_guid::actordataid(adata.m_roleid()));
			lrobot.m_account = adata.m_account();
			lrobot.m_actor_roleid = actor_guid::make_type(lrobot.m_robot->id_guid(), ACTOR_ROLE);

			connect(adata.m_gatewayid(), [adata, &lrobot, this](int asession)
				{
					std::cout << "#############[" << adata.m_account() << "]已连接[GateWay]["<< asession <<"]#############" << std::endl;
					lrobot.m_session = asession;

					pbnet::PROBUFF_NET_ROLE_LOGIN pro;
					pro.set_m_roleid(adata.m_roleid());
					pro.set_m_session(adata.m_session());
					pro.set_m_area(adata.m_area());
					pro.set_m_iscreate(false);
					pro.set_m_gatewayid(adata.m_gatewayid());
					nserver->send(asession, pro, actor_guid::moreactor(), this->id_guid());
				});
			
			return true;
		}

	public:

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, robot_pram& adata)
		{
			if (adata.m_parm.size() > 1)
			{
				std::transform(adata.m_parm[1].begin(), adata.m_parm[1].end(), adata.m_parm[1].begin(), tolower);
			}
			if (adata.m_parm[0] == "logins" || adata.m_parm[0] == "LOGINS")
			{
				create_robots(adata.m_parm[1], boost::lexical_cast<int>(adata.m_parm[2]), boost::lexical_cast<int>(adata.m_parm[3]));
			}
			else if (adata.m_parm[0] == "login" || adata.m_parm[0] == "LOGIN")
			{
				create_robot(adata.m_parm[1]);
			}
			else if (adata.m_parm[0] == "c")
			{//c libo1 1
				pbnet::PROBUFF_NET_CMD pro;
				std::string lstr;
				for (int i = 2; i < adata.m_parm.size(); ++i)
				{
					if (i == 3)
						lstr += '|';
					else if(i >= 4)
						lstr += '*';
					lstr += adata.m_parm[i];
				}
				pro.set_m_cmd(lstr);
				send(get_robot(adata.m_parm[1]), pro);
			}
			else if (adata.m_parm[0] == "C")
			{//C 1
				pbnet::PROBUFF_NET_CMD pro;
				std::string lstr;
				for (int i = 1; i < adata.m_parm.size(); ++i)
				{
					if (i == 2)
						lstr += '|';
					else if (i >= 3)
						lstr += '*';
					lstr += adata.m_parm[i];
				}
				pro.set_m_cmd(lstr);
				foreach([&pro,this](actor_manage_robot::_robot& arobot)
					{
						send(&arobot, pro);
						return true;
					});
			}
			else if (adata.m_parm[0] == "X")
			{
				//pbnet::PROBUFF_NET_GET_TIME pro;
				//foreach([&pro, this](actor_manage_robot::_robot& arobot)
				//	{
				//		send(&arobot, pro);
				//		return true;
				//	});	
				pbnet::PROBUFF_NET_CHAT pro;
				pro.set_m_type(1);
				pro.set_m_channelid(1);
				pro.set_m_content("ribenrensb3");
				foreach([&pro, this](actor_manage_robot::_robot& arobot)
					{
						send(&arobot, pro);
						return true;
					});	

			}
			else if (adata.m_parm[0] == "X1")
			{
				pbnet::PROBUFF_NET_GET_TIME pro;
				foreach([&pro, this](actor_manage_robot::_robot& arobot)
					{
						send(&arobot, pro);
						return true;
					});	
			}
			return true;
		}
		
		void create_robots(std::string& arobotname, int abeg, int aend)
		{
			for (int i = abeg; i <= aend; ++i)
			{
				std::string lname(arobotname);
				lname += "";
				lname += boost::lexical_cast<std::string>(i);
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
			nserver->send(arobot->m_session, adata, actor_guid::moreactor(), arobot->m_actor_roleid);
		}

		bool getdata(_robot* arobot)
		{
			if (arobot == nullptr)
				return false;
			//LogLocalInfo("[%]## [%]", arobot->m_account, arobot->m_robot->get());
			return true;
		}

		bool switchline(_robot* arobot, int aline)
		{
			//LOGIC_SWITCH_LINE pro;
			//pro.m_line = aline;
			//nserver->send(arobot->m_session, pro, actor_guid::moreactor(), id_guid());
			return true;
		}
	};
}
	
