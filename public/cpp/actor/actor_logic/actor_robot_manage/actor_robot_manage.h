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
// File overview: Declares interfaces for actor robot manage.

#pragma once

#include "actor/actor_logic/actor_robot/actor_robot.h"
#include "net/nnet.h"

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

		static void login(const std::string& aaccount, const std::string& apasswold, ENET_PROTOCOL aprotocol = ENET_TCP);

		bool check_connect(i32_serverid aserverid, ENET_PROTOCOL aprotocol) const;

		bool connect(i32_serverid aserverid, ENET_PROTOCOL aprotocol, const std::function<void(i32_sessionid)>& afun) const;

		static bool parse_command(std::string aparm);

		void create_robots(const std::string& arobotname, int abeg, int aend, ENET_PROTOCOL aprotocol = ENET_TCP) const;

		void create_robot(const std::string& arobotname, ENET_PROTOCOL aprotocol = ENET_TCP) const;

		template <typename TFun>
		void foreach(const TFun& afun)
		{
			for (auto& lentry : m_maprobot)
			{
				afun(lentry.second);
			}
		}

		_robot* get_robot(const std::string& aacount);
		_robot* get_robot(i64_actorid aroleid);

		template <typename T>
		void send(_robot* arobot, const T& adata)
		{
			if (arobot == nullptr)
			{
				return;
			}
			nnet::instance().send(arobot->m_session, adata, nguid::moreactor(), arobot->m_actor_roleid);
		}

		bool getdata(_robot* arobot);

		bool kcp_connect(i64_actorid arobotid, pbnet::ENUM_KCP akcpenum, int16_t aservertid, int16_t atcount, i64_actorid aseractorid);

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		void help();

		bool handle([[maybe_unused]] const message<np_robot_pram>& adata);
		bool handle([[maybe_unused]] const message<np_ukcp_waitrecv>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLE_NOT_CREATE>& adata);
	};
}//namespace ngl
