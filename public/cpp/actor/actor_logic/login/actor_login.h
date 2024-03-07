#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "uuid.h"
#include "account.h"
#include "db.pb.h"
#include "net.pb.h"

namespace ngl
{
	class actor_login : public actor
	{
		struct server_info
		{
			int32_t m_id = 0;
			int32_t m_rolesize = 0;
			def_portocol_function_log(m_id, m_rolesize)
		};
		// ----- Data Begin -----
		account m_account;
		std::map<i32_serverid, server_info> m_game;
		std::map<i32_serverid, server_info> m_gateway;
		i16_area m_config_area;
		struct pair_account
		{
			i32_serverid m_gameserverid;
			i32_serverid m_gatewayserverid;
			std::string m_session;
		};
		std::map<i64_actorid, pair_account> m_actorbyserver;
		// ----- Data End   -----
		actor_login();

	public:
		friend class actor_instance<actor_login>;
		static actor_login& getInstance() 
		{ 
			return actor_instance<actor_login>::instance(); 
		}

		virtual void init();

		virtual ~actor_login();

		static void nregister();

		virtual void loaddb_finish(bool adbishave);

		data_modified<pbdb::db_account>* get_account(int area, const std::string& account, const std::string& apassworld, bool& aiscreate);

		bool get_freeserver(std::map<i32_serverid, server_info>& amap, std::pair<i32_serverid, int32_t>& apair);

		bool get_freeserver_game(std::pair<i32_serverid, int32_t>& apair);

		bool get_freeserver_gateway(std::pair<int32_t, int32_t>& apair);

		bool dec_freeserver(std::map<i32_serverid, server_info>& amap, i32_serverid aserverid);

		bool dec_freeserver_game(i32_serverid aserverid);

		bool dec_freeserver_gateway(i32_serverid aserverid);

		void printf_freeserver();

		
		bool handle(message<actor_server_connect>& adata);

		bool handle(message<pbnet::PROBUFF_NET_ACOUNT_LOGIN>& adata);

		bool handle(message<actor_role_login>& adata);
	
		bool handle(message<actor_disconnect_close>& adata);
	};

}
