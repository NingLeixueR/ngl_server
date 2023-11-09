#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_register.h"
#include "cache_list.h"
#include "ttab_dbload.h"

namespace ngl
{
	class actor_dbpool
	{
		static std::vector<db*> m_data;
		static bool m_init;
	public:
		static void init();
		static db* get(int apos);
	};

	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	class actor_dbtab
	{
		static tab_dbload* m_tab;
		// ## 加载出id 防止内存穿透
		static std::set<int64_t> m_idset;

		template <typename TDB>
		static void cachelist(enum_cache_list atype, std::set<i64_actorid>& aset);
	public:
		static void init()
		{
			Try
			{
				if (m_tab == nullptr)
				{
					m_tab = ttab_dbload::get_tabdb<TDBTAB>();
					Assert(m_tab != nullptr);
				}
				cache_list<TDBTAB, enum_clist_save>::getInstance().set_cachefun(
					boost::bind(&actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::cachelist<TDBTAB>, enum_clist_save, std::placeholders::_1));

				cache_list<TDBTAB, enum_clist_del>::getInstance().set_cachefun(
					boost::bind(&actor_dbtab<PROTYPE,TDBTAB_TYPE, TDBTAB>::cachelist<TDBTAB>, enum_clist_del, std::placeholders::_1));

				if (m_tab->m_isloadall == true)
				{
					db_manage::select<PROTYPE,TDBTAB>::fun(actor_dbpool::get(0));
					ngl::dbdata<TDBTAB>::foreach([](TDBTAB& adata)
						{
							if constexpr(PROTYPE == EPROTOCOL_TYPE_PROTOCOLBUFF)
							{
								actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::m_idset.insert(adata.m_id());
							}
							if constexpr (PROTYPE == EPROTOCOL_TYPE_CUSTOM)
							{
								actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::m_idset.insert(adata.const_mm_id());
							}
						});
				}
				else
				{
					db_manage::select<PROTYPE, TDBTAB>::fun(actor_dbpool::get(0), m_idset);
				}
			}Catch;
		}

		static void loadall(const std::shared_ptr<pack>& apack, const actor_db_load<PROTYPE, TDBTAB_TYPE, TDBTAB>& adata)
		{
			if (!m_tab->m_network) return;
			int lsendmaxcount = m_tab->m_sendmaxcount;
			//加载全部数据
			Assert(m_tab->m_isloadall);
			i64_actorid lrequestactor = apack->m_head.get_request_actor();

			std::shared_ptr<actor_db_load_response<PROTYPE, TDBTAB_TYPE, TDBTAB>> pro(new actor_db_load_response<PROTYPE, TDBTAB_TYPE, TDBTAB>
				{
					.m_stat = true,
					.m_over = false,
				});
			pro->m_data.make();
			ngl::dbdata<TDBTAB>::foreach_index([lrequestactor, lsendmaxcount, &pro](int aindex, TDBTAB& atab)
				{

					if constexpr (PROTYPE == EPROTOCOL_TYPE_CUSTOM)
					{
						actor_guid lguid(atab.const_mm_id());
						pro->m_data.insert(std::make_pair(lguid, atab));
						if (aindex % lsendmaxcount == 0)
						{
							actor::static_send_actor(lrequestactor, actor_guid::make(), pro);
							pro = std::shared_ptr<actor_db_load_response<PROTYPE, TDBTAB_TYPE, TDBTAB>>(new actor_db_load_response<TDBTAB_TYPE, TDBTAB>
								{
									.m_stat = true,
									.m_over = false,
								});
						}
					}
					if constexpr (PROTYPE == EPROTOCOL_TYPE_PROTOCOLBUFF)
					{
						actor_guid lguid(atab.m_id());
						pro->m_data.m_data->insert(std::make_pair(lguid, atab));
						if (aindex % lsendmaxcount == 0)
						{
							actor::static_send_actor(lrequestactor, actor_guid::make(), pro);
							pro = std::shared_ptr<actor_db_load_response<PROTYPE, TDBTAB_TYPE, TDBTAB>>(new actor_db_load_response<PROTYPE, TDBTAB_TYPE, TDBTAB>
								{
									.m_stat = true,
									.m_over = false,
								});
							pro->m_data.make();
						}
					}
					
				});
			pro->m_over = true;
			actor::static_send_actor(lrequestactor, actor_guid::make(), pro);
			LogLocalInfo("loadall[%]", TDBTAB().descriptor()->full_name());
		}

		static void load(i32_threadid athreadid, int64_t aid)
		{
			if (aid == -1)
				return;
			if (ngl::dbdata<TDBTAB>::isload(aid) == false)
				db_manage::select<PROTYPE, TDBTAB>::fun(actor_dbpool::get(athreadid), aid);
		}

		static void load(i32_threadid athreadid, const std::shared_ptr<pack>& apack, const actor_db_load<PROTYPE, TDBTAB_TYPE, TDBTAB>& adata)
		{
			if (!m_tab->m_network)
				return;
			i64_actorid lid = adata.m_id.id();
			if (lid == -1)
			{
				//加载全部数据
				Try
				{
					Assert(m_tab->m_isloadall);
					loadall(apack, adata);
				}Catch;
			}
			else
			{
				if (m_idset.find(lid) == m_idset.end())
					return;
				load(athreadid, lid);
				auto pro = std::make_shared<actor_db_load_response<PROTYPE, TDBTAB_TYPE, TDBTAB>>();
				pro->m_data.make();
				pro->m_over = true;
				if constexpr (PROTYPE == EPROTOCOL_TYPE_CUSTOM)
				{
					pro->m_stat = ngl::dbdata<TDBTAB>::get(lid, pro->m_data[adata.m_id]);
				}
				if constexpr (PROTYPE == EPROTOCOL_TYPE_PROTOCOLBUFF)
				{
					pro->m_stat = ngl::dbdata<TDBTAB>::get(lid, (*pro->m_data.m_data)[adata.m_id]);
				}
				
				uint64_t lrequestactor = apack->m_head.get_request_actor();
				actor::static_send_actor(lrequestactor, actor_guid::make(), pro);
			}
		}

		static void save(i32_threadid athreadid, const TDBTAB& adata)
		{
			if constexpr (PROTYPE == EPROTOCOL_TYPE_CUSTOM)
			{
				m_idset.insert(adata.const_mm_id());
				ngl::dbdata<TDBTAB>::set(adata.const_mm_id(), adata);
				cache_list<TDBTAB, enum_clist_save>::getInstance().push(adata.const_mm_id());
			}
			if constexpr (PROTYPE == EPROTOCOL_TYPE_PROTOCOLBUFF)
			{
				m_idset.insert(adata.m_id());
				ngl::dbdata<TDBTAB>::set(adata.m_id(), adata);
				cache_list<TDBTAB, enum_clist_save>::getInstance().push(adata.m_id());
			}
		}

		static void del(i32_threadid athreadid, i64_actorid aid)
		{
			m_idset.erase(aid);
			ngl::dbdata<TDBTAB>::remove(aid);
			cache_list<TDBTAB, enum_clist_del>::getInstance().push(aid);
		}

		static void del(i32_threadid athreadid, std::vector<i64_actorid>& aid)
		{
			for (i64_actorid id: aid)
			{
				m_idset.erase(id);
			}
			ngl::dbdata<TDBTAB>::remove(aid);
			cache_list<TDBTAB, enum_clist_del>::getInstance().push(aid);
		}

		static void save(i32_threadid athreadid, const std::shared_ptr<pack>& apack, const actor_db_save<PROTYPE, TDBTAB_TYPE, TDBTAB>& adata)
		{
			if constexpr (PROTYPE == EPROTOCOL_TYPE_CUSTOM)
			{
				const std::map<actor_guid, TDBTAB>& lmap = adata.m_data;
				for (const std::pair<const actor_guid, TDBTAB>& item : lmap)
				{
					m_idset.insert(item.first.id());
					save(athreadid, item.second);
				}
			}
			if constexpr (PROTYPE == EPROTOCOL_TYPE_PROTOCOLBUFF)
			{
				const std::map<actor_guid, TDBTAB>& lmap = *adata.m_data.m_data;
				for (const std::pair<const actor_guid, TDBTAB>& item : lmap)
				{
					m_idset.insert(item.first.id());
					save(athreadid, item.second);
				}
			}
			
		}
	};

	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	tab_dbload* actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::m_tab = nullptr;

	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	std::set<int64_t> actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::m_idset;

	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	class actor_db : public actor
	{
	public:
		enum { ACTOR_TYPE = PROTYPE * 1000 + ACTOR_DB + TDBTAB_TYPE };
	private:
		actor_db() :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = (ENUM_ACTOR)(ACTOR_TYPE), 
						.m_area = ttab_servers::tab()->m_area,
						.m_id = nconfig::m_nodeid
					},
					.m_weight = 0x7fffffff,
				})
		{
			actor_dbpool::init();
			actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::init();
		}
	public:
		//db m_db;
		friend class actor_instance<actor_db<PROTYPE, TDBTAB_TYPE, TDBTAB>>;
		static actor_db<PROTYPE, TDBTAB_TYPE, TDBTAB>& getInstance()
		{
			return actor_instance<actor_db<PROTYPE, TDBTAB_TYPE, TDBTAB>>::instance();
		}

		virtual ~actor_db() {}

		static void actor_register()
		{
			EPROTOCOL_TYPE ltype = PROTYPE;
			using TDerived = actor_db<PROTYPE, TDBTAB_TYPE, TDBTAB>;
			TDerived::template register_actor<PROTYPE, TDerived>(
				true,
				null<actor_db_load<PROTYPE, TDBTAB_TYPE, TDBTAB>>,
				null<actor_db_save<PROTYPE, TDBTAB_TYPE, TDBTAB>>,
				null<actor_db_delete<PROTYPE, TDBTAB_TYPE, TDBTAB>>,
				null<actor_time_db_cache<PROTYPE, TDBTAB>>
				);
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, 
			actor_db_load<PROTYPE, TDBTAB_TYPE, TDBTAB>& adata)
		{
			//LogLocalInfo(" actor_db actor_db_load<%,%> [%]", TDBTAB_TYPE, TDBTAB::name(), adata.m_id);
			actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::load(athread, apack, adata);
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, 
			actor_db_save<PROTYPE, TDBTAB_TYPE, TDBTAB>& adata)
		{
			//for (auto& [key, value] : adata.m_data)
			//	LogLocalInfo(" actor_db actor_db_save<%,%,false> [%:%:%]", TDBTAB_TYPE, TDBTAB::name(), key.type(), key.area(), key.actordataid());
			actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::save(athread, apack, adata);
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, 
			actor_db_delete<PROTYPE, TDBTAB_TYPE, TDBTAB>& adata)
		{
			//for (auto& id : adata.m_data)
			//	LogLocalInfo(" actor_db actor_db_delete<%,%,false> [%]", TDBTAB_TYPE, TDBTAB::name(), id);
			actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::del(athread, adata.m_data);
			return true;
		}

		////// ----ACTOR_TIMER_DB_CACHE, db cache list  保存缓存列表
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, 
			actor_time_db_cache<PROTYPE, TDBTAB>& adata)
		{
			for (i64_actorid id : adata.m_ls)
			{
				switch (adata.m_type)
				{
				case enum_clist_save:
				{
					if (ngl::dbdata<TDBTAB>::find(id) == nullptr)
						continue;
					db_manage::save<PROTYPE, TDBTAB>::fun(actor_dbpool::get(athread), id);
				}
				break;
				case enum_clist_del:
				{
					ngl::dbdata<TDBTAB>::remove(id);
					db_manage::del<PROTYPE, TDBTAB>::fun(actor_dbpool::get(athread), id);
				}
				break;
				}
			}
			return true;
		}
	};

	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	template <typename TDB>
	static void actor_dbtab<PROTYPE, TDBTAB_TYPE, TDBTAB>::cachelist(enum_cache_list atype, std::set<i64_actorid>& aset)
	{
		if (aset.empty())
			return;
		std::shared_ptr<actor_time_db_cache<PROTYPE, TDB>> pro(new actor_time_db_cache<PROTYPE, TDB>());
		pro->m_type = atype;
		pro->m_ls.swap(aset);

		i64_actorid lactorid = actor_guid::make(
			(ENUM_ACTOR)(actor_db<PROTYPE, TDBTAB_TYPE, TDBTAB>::ACTOR_TYPE),
			tab_self_area,
			nconfig::m_nodeid);
		actor_base::static_send_actor(lactorid, actor_guid::make(), pro);
	}
}
