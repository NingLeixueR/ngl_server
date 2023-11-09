#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "uuid.h"
//#include "key_value.h"
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
		// ----- Data End   -----
		actor_login() :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = ACTOR_LOGIN, 
						.m_area = ttab_servers::tab()->m_area,
						.m_id = nconfig::m_nodeid, 
						.m_manage_dbclient = true
					},
					.m_weight = 0x7fffffff
				}),
			m_config_area(0)
		{
			m_config_area = ttab_servers::tab()->m_area;
		}

	public:
		friend class actor_instance<actor_login>;
		static actor_login& getInstance() 
		{ 
			return actor_instance<actor_login>::instance(); 
		}
		virtual void init()
		{
			m_account.set(this);
		}

		virtual ~actor_login() {}

		static void actor_register();

		enum { ACTOR_TYPE = ACTOR_LOGIN};

		virtual void loaddb_finish(bool adbishave)
		{
			for (auto& item : m_account.data())
			{
				const pbdb::db_account& laccount = item.second.getconst();
				LogLocalError("[%][%] area=% id=% ",
					laccount.m_account(),
					laccount.m_passworld(),
					laccount.m_passworld(),
					laccount.m_area(),
					actor_guid(laccount.m_roleid()).actordataid()
				);
			}
			
		}

		data_modified<pbdb::db_account>* get_account(int area, const std::string& account,const std::string& apassworld, bool& aiscreate)
		{
			aiscreate = false;
			auto& lmap = m_account.accountbyaccount(area);
			auto itor = lmap.find(account);
			if (itor != lmap.end())
			{
				if(itor->second->getconst().m_passworld() != apassworld)
					return nullptr;
				return itor->second;
			}
			else
			{
				pbdb::db_account laccount;
				laccount.set_m_account(account);
				laccount.set_m_passworld(apassworld);
				int64_t lid = actor_guid::make(ACTOR_ROLE, m_config_area, ++m_account.keyvalue_accountid());
				laccount.set_m_id(lid);
				laccount.set_m_roleid(lid);

				data_modified<pbdb::db_account>* lpaccount = m_account.add(lid, laccount);
				lmap[account] = lpaccount;
				aiscreate = true;
				return lpaccount;
			}
		}

		bool get_freeserver(std::map<i32_serverid, server_info>& amap, std::pair<i32_serverid, int32_t>& apair)
		{
			apair.first = -1;
			apair.second = -1;
			server_info* lpinfo = nullptr;
			for (auto&& [serverid, serverin] : amap)
			{
				if (apair.second == -1 || apair.second > serverin.m_rolesize)
				{
					apair.first = serverin.m_id;
					apair.second = serverin.m_rolesize;
					lpinfo = &serverin;
				}
			}
			if (lpinfo != nullptr)
			{
				++lpinfo->m_rolesize;
			}
			return apair.first > 0;
		}

		bool get_freeserver_game(std::pair<i32_serverid, int32_t>& apair)
		{
			return get_freeserver(m_game, apair);
		}

		bool get_freeserver_gateway(std::pair<int32_t, int32_t>& apair)
		{
			return get_freeserver(m_gateway, apair);
		}

		bool dec_freeserver(std::map<i32_serverid, server_info>& amap, i32_serverid aserverid)
		{
			auto itor = amap.find(aserverid);
			if (itor == amap.end())
				return false;
			--itor->second.m_rolesize;
			return true;
		}

		bool dec_freeserver_game(i32_serverid aserverid)
		{
			return dec_freeserver(m_game, aserverid);
		}

		bool dec_freeserver_gateway(i32_serverid aserverid)
		{
			return dec_freeserver(m_gateway, aserverid);
		}

		void printf_freeserver()
		{
			LogLocalError("game[%] \ngateway[%]", m_game, m_gateway);
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_server_connect& adata)
		{
			server_info ltemp
			{
				.m_id = adata.m_serverid,
				.m_rolesize = 0
			};
			switch (ttab_servers::node_type(adata.m_serverid))
			{
			case ngl::NODE_TYPE::GAME:
				m_game.insert(std::make_pair(adata.m_serverid, ltemp));
				break;
			case ngl::NODE_TYPE::GATEWAY:
				m_gateway.insert(std::make_pair(adata.m_serverid, ltemp));
				break;
			}	
			return true;
		}

		struct pair_account
		{
			i32_serverid m_gameserverid;
			i32_serverid m_gatewayserverid;
			std::string m_session;
		};

		std::map<i64_actorid, pair_account> m_actorbyserver;

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata)
		{
			Try
			{
				Assert(apack != nullptr);
				LogLocalInfo("############ Login[%][%][%] ############", adata.m_area(), adata.m_account(), adata.m_password());
				bool iscreate = false;

				data_modified<pbdb::db_account>* lpaccount = get_account(adata.m_area(), adata.m_account(), adata.m_password(), iscreate);
				Assert(lpaccount != nullptr);


				pair_account* lppair_account = nullptr;
				auto itor = m_actorbyserver.find(lpaccount->getconst().m_id());
				if (itor == m_actorbyserver.end())
				{
					lppair_account = &m_actorbyserver[lpaccount->getconst().m_id()];
					// 生成session key
					ngl::uuid::create(lppair_account->m_session);

					std::pair<int32_t, int32_t> lpairgame;
					std::pair<int32_t, int32_t> lpairgateway;
					Assert(get_freeserver_game(lpairgame));
					Assert(get_freeserver_gateway(lpairgateway));
					lppair_account->m_gameserverid = lpairgame.first;
					lppair_account->m_gatewayserverid = lpairgateway.first;
				}
				else
				{
					lppair_account = &itor->second;
				}

				// 通知gateway服务器				
				actor_role_login pro
				{
					.m_session = lppair_account->m_session,
					.m_accountid = lpaccount->getconst().m_id(),
					.m_roleid = lpaccount->getconst().m_roleid(),
					.m_gameid = lppair_account->m_gameserverid,
					.m_gatewayid = lppair_account->m_gatewayserverid,
					.m_area = (int16_t)lpaccount->getconst().m_area(),
					.m_iscreate = iscreate,
				};
				nserver->sendtoserver(pro.m_gatewayid, pro, actor_guid::moreactor(), id_guid());

				{// 回复客户端
					pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE pro;
					pro.set_m_roleid(lpaccount->getconst().m_id());
					pro.set_m_area(lpaccount->getconst().m_area());
					pro.set_m_session(lppair_account->m_session);
					pro.set_m_account(adata.m_account());
					pro.set_m_gatewayid(lppair_account->m_gatewayserverid);
					nserver->send(apack->m_id, pro, apack->m_head.get_request_actor(), id_guid());
				}

			}Catch;
			return true;
		}
	
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_disconnect_close& adata)
		{
			auto itor = m_actorbyserver.find(adata.m_actorid);
			if (itor == m_actorbyserver.end())
				return true;
			dec_freeserver_game(itor->second.m_gameserverid);
			dec_freeserver_gateway(itor->second.m_gatewayserverid);
			m_actorbyserver.erase(itor);

			printf_freeserver();

			return true;
		}
	};

}
