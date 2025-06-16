#pragma once

#include "actor_robot.h"

namespace ngl
{
	struct _robot
	{
		i32_sessionid					m_session = -1;
		std::string						m_account;
		std::shared_ptr<actor_robot>	m_robot = nullptr;
		i64_actorid						m_actor_roleid = nguid::make();
	};

	class actor_robot_manage : 
		public actor
	{
		actor_robot_manage();
	public:

		std::map<std::string, _robot> m_maprobot;

		friend class actor_instance<actor_robot_manage>;
		static actor_robot_manage& instance() 
		{ 
			return actor_instance<actor_robot_manage>::instance();
		}

		virtual ~actor_robot_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		std::shared_ptr<actor_robot> create(i16_area aarea, i32_actordataid aroleid);

		static void login(const std::string& aaccount, const std::string& apasswold);

		bool check_connect(i32_serverid aserverid)const;

		void connect(i32_serverid aserverid, const std::function<void(i32_sessionid)>& afun) const;

		static bool parse_command(std::vector<std::string>& aparm);

		void create_robots(const std::string& arobotname, int abeg, int aend);

		void create_robot(const std::string& arobotname);

		void foreach(const std::function<bool(_robot&)>& afun);

		_robot* get_robot(std::string aacount);

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

		bool getdata(_robot* arobot);

		using handle_cmd = cmd<actor_robot_manage, std::string, const std::vector<std::string>&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<np_robot_pram>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>& adata);
	};
}//namespace ngl
	
