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
#pragma once

#include "actor_robot.h"

namespace ngl
{
	class actor_robot_manage : 
		public actor
	{
		actor_robot_manage();
	public:
		std::map<std::string, _robot> m_maprobot;
		std::map<i64_actorid, _robot*> m_maprobotbyactorid;

		friend class actor_instance<actor_robot_manage>;
		static actor_robot_manage& instance() 
		{ 
			return actor_instance<actor_robot_manage>::instance();
		}

		virtual ~actor_robot_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		std::shared_ptr<actor_robot> create(i16_area aarea, i32_actordataid aroleid);

		static void login(const std::string& aaccount, const std::string& apasswold);

		bool check_connect(i32_serverid aserverid)const;

		void connect(i32_serverid aserverid, const std::function<void(i32_sessionid)>& afun) const;

		static bool parse_command(std::vector<std::string>& aparm);

		void create_robots(const std::string& arobotname, int abeg, int aend)const;

		void create_robot(const std::string& arobotname)const;

		void foreach(const std::function<bool(_robot&)>& afun);

		_robot* get_robot(std::string aacount);
		_robot* get_robot(i64_actorid aroleid);

		template <typename T>
		void send(_robot* arobot, T& adata)
		{
			nets::send(arobot->m_session, adata, nguid::moreactor(), arobot->m_actor_roleid);
		}

		bool getdata(_robot* arobot);

		bool kcp_connect(i64_actorid arobotid, pbnet::ENUM_KCP akcpenum, int16_t aservertid, int16_t atcount, i64_actorid aseractorid);

		using handle_cmd = cmd<actor_robot_manage, std::string, const std::vector<std::string>&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_robot_pram>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_NOT_CREATE>& adata);
	};
}//namespace ngl
