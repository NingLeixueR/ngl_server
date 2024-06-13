#pragma once

#include "actor_manage.h"
#include "ttab_dbload.h"
#include "cache_list.h"
#include "ndbclient.h"
#include "nregister.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "gcmd.h"
#include "cmd.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	// db连接池
	class actor_dbpool
	{
		static std::vector<db*> m_data;
		static bool m_init;
	public:
		static void init();
		static db* get(int apos);
	};

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	class actor_dbtab
	{
		static tab_dbload*			m_tab;
		// # 加载出id 防止内存穿透
		static std::set<int64_t>	m_idset;

		template <typename TDB>
		static void cachelist(enum_cache_list atype, std::set<i64_actorid>& aset);

		template <enum_cache_list ENUM>
		static cache_list<TDBTAB, ENUM>& inst()
		{
			return cache_list<TDBTAB, ENUM>::getInstance();
		}
	public:
		using type_actor_dbtab = actor_dbtab<TDBTAB_TYPE, TDBTAB>;

		// # 初始化
		static void init()
		{
			Try
			{
				m_tab = ttab_dbload::get_tabdb<TDBTAB>();
				Assert(m_tab != nullptr);

				db_data<TDBTAB>::init();

				inst<enum_clist_save>().
					set_cachefun(
						std::bind(&cachelist<TDBTAB>, enum_clist_save, std::placeholders::_1)
					);

				inst<enum_clist_del>().
					set_cachefun(
						std::bind(&cachelist<TDBTAB>, enum_clist_del, std::placeholders::_1)
					);

				if (m_tab->m_isloadall == true)
				{
					db_manage::select<TDBTAB>::fun(actor_dbpool::get(0));
					ngl::db_data<TDBTAB>::foreach([](TDBTAB& adata)
						{
							type_actor_dbtab::m_idset.insert(
								adata.m_id()
							);
						});
				}
				else
				{
					db_manage::select<TDBTAB>::fun(actor_dbpool::get(0), m_idset);
				}
			}Catch;
		}

		// # 加载表中的所有数据
		static void loadall(const pack* apack, const np_actordb_load<TDBTAB_TYPE, TDBTAB>& adata)
		{
			if (!m_tab->m_network) 
				return;
			int lsendmaxcount = m_tab->m_sendmaxcount;
			if (lsendmaxcount <= 0)
				lsendmaxcount = 10;
			//加载全部数据
			Assert(m_tab->m_isloadall);
			i64_actorid lrequestactor = apack->m_head.get_request_actor();
			np_actordb_load_response<TDBTAB_TYPE, TDBTAB> pro;
			pro.m_stat = true;
			pro.m_over = false;
			pro.m_data.make();
			ngl::db_data<TDBTAB>::foreach_index(
				[lrequestactor, lsendmaxcount, apack, &pro](int aindex, TDBTAB& atab)
				{
					nguid lguid(atab.m_id());
					pro.m_data.m_data->insert(std::make_pair(lguid, atab));
					if (aindex % lsendmaxcount == 0)
					{
						nets::sendbysession(apack->m_id, pro, lrequestactor, nguid::make());
						pro = np_actordb_load_response<TDBTAB_TYPE, TDBTAB>();
						pro.m_stat = true;
						pro.m_over = false;
						pro.m_data.make();
					}
				});
			pro.m_over = true;
			nets::sendbysession(apack->m_id, pro, lrequestactor, nguid::make());
			log_info()->print("loadall[{}]", TDBTAB().descriptor()->full_name());
		}

		// # 加载表中的指定数据
		static void load(i32_threadid athreadid, int64_t aid)
		{
			if (aid == -1)
				return;
			if (ngl::db_data<TDBTAB>::isload(aid) == false)
				db_manage::select<TDBTAB>::fun(actor_dbpool::get(athreadid), aid);
		}

		// # 加载数据 ：同步方式
		static void load(i32_threadid athreadid, const pack* apack, const np_actordb_load<TDBTAB_TYPE, TDBTAB>& adata)
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
				{
					log_error()->print("load <<{}>>===<<{}>>", dtype_name(type_actor_dbtab), lid);
					return;
				}
					
				load(athreadid, lid);
				
				np_actordb_load_response<TDBTAB_TYPE, TDBTAB> pro;
				pro.m_data.make();
				pro.m_over = true;
				pro.m_stat = ngl::db_data<TDBTAB>::get(lid, (*pro.m_data.m_data)[adata.m_id]);
				i64_actorid lrequestactor = apack->m_head.get_request_actor();
				nets::sendbysession(apack->m_id, pro, lrequestactor, nguid::make());

				using type_message = np_actordb_load<TDBTAB_TYPE, TDBTAB>;
				log_error()->print("load finish: [{}][{}]", lrequestactor, dtype_name(type_message));
			}
		}

		// # 异步保存数据  将需要保存的数据添加到缓存保存队列
		static void save(i32_threadid athreadid, const TDBTAB& adata)
		{
			int64_t lid = adata.m_id();
			m_idset.insert(lid);
			ngl::db_data<TDBTAB>::set(lid, adata);
			inst<enum_clist_save>().push(lid);
		}

		// # 异步删除数据  将需要删除的数据添加到缓存保存队列
		static void del(i32_threadid athreadid, i64_actorid aid)
		{
			m_idset.erase(aid);
			ngl::db_data<TDBTAB>::remove(aid);
			inst<enum_clist_del>().push(aid);
		}

		static void del(i32_threadid athreadid, std::vector<i64_actorid>& aid)
		{
			for (i64_actorid id: aid)
			{
				m_idset.erase(id);
			}
			ngl::db_data<TDBTAB>::remove(aid);
			inst<enum_clist_del>().push(aid);
		}

		static void save(i32_threadid athreadid, const pack* apack, const np_actordb_save<TDBTAB_TYPE, TDBTAB>& adata)
		{
			const std::map<nguid, TDBTAB>& lmap = *adata.m_data.m_data;
			for (const std::pair<const nguid, TDBTAB>& item : lmap)
			{
				m_idset.insert(item.first.id());
				save(athreadid, item.second);
			}
		}
	};

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	tab_dbload* actor_dbtab<TDBTAB_TYPE, TDBTAB>::m_tab = nullptr;

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	std::set<int64_t> actor_dbtab<TDBTAB_TYPE, TDBTAB>::m_idset;

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	class actor_db : public actor
	{
		using tactor_db = actor_db<TDBTAB_TYPE, TDBTAB>;
	private:

		actor_db(const actor_db&) = delete;
		actor_db& operator=(const actor_db&) = delete;

		actor_db() :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = nactor_type<tactor_db>::type(),
						.m_area = ttab_servers::tab()->m_area,
					},
					.m_weight = 0x7fffffff,
				})
		{
			actor_dbpool::init();
			actor_dbtab<TDBTAB_TYPE, TDBTAB>::init();
		}
	public:
		friend class actor_instance<tactor_db>;
		static tactor_db& getInstance()
		{
			return actor_instance<tactor_db>::instance();
		}

		virtual ~actor_db() 
		{
		}

		static void nregister()
		{
			actor::register_handle_proto<tactor_db>::template func<
				np_actordb_load<TDBTAB_TYPE, TDBTAB>
				, np_actordb_save<TDBTAB_TYPE, TDBTAB>
				, np_actordb_delete<TDBTAB_TYPE, TDBTAB>
				, np_actortime_db_cache<TDBTAB>
			>(true);
			actor::register_handle_custom<tactor_db>::template func<
				mforward<np_gm>
			>(true);
		}

		bool handle(message<np_actordb_load<TDBTAB_TYPE, TDBTAB>>& adata)
		{
			using type_message = np_actordb_load<TDBTAB_TYPE, TDBTAB>;
			log_error()->print("load: [{}] [{}]", dtype_name(type_message), adata.m_data->m_id);
			actor_dbtab<TDBTAB_TYPE, TDBTAB>::load(adata.m_thread, adata.m_pack, *adata.m_data);
			return true;
		}

		bool handle(message<np_actordb_save<TDBTAB_TYPE, TDBTAB>>& adata)
		{
			actor_dbtab<TDBTAB_TYPE, TDBTAB>::save(adata.m_thread, adata.m_pack, *adata.m_data);
			return true;
		}

		bool handle(message<np_actordb_delete<TDBTAB_TYPE, TDBTAB>>& adata)
		{
			actor_dbtab<TDBTAB_TYPE, TDBTAB>::del(adata.m_thread, adata.m_data->m_data);
			return true;
		}

		// # ACTOR_TIMER_DB_CACHE, db cache list  保存缓存列表
		bool handle(message<np_actortime_db_cache<TDBTAB>>& adata)
		{
			auto lrecv = adata.m_data;
			for (i64_actorid id : lrecv->m_ls)
			{
				switch (lrecv->m_type)
				{
				case enum_clist_save:
				{
					if (ngl::db_data<TDBTAB>::find(id) == nullptr)
						continue;
					db_manage::save<TDBTAB>::fun(actor_dbpool::get(adata.m_thread), id);
				}
				break;
				case enum_clist_del:
				{
					ngl::db_data<TDBTAB>::remove(id);
					db_manage::del<TDBTAB>::fun(actor_dbpool::get(adata.m_thread), id);
				}
				break;
				}
			}
			return true;
		}

		using handle_cmd = cmd<tactor_db, std::string, int, int, ngl::json_read&>;

		bool handle(message<mforward<np_gm>>& adata)
		{
			ngl::json_read lojson(adata.m_data->data()->m_json.c_str());

			std::string loperator;
			if (lojson.read("operator", loperator) == false)
				return true;

			if (handle_cmd::empty())
			{
				handle_cmd::push("query", [this](int athread, int id, ngl::json_read& aos)
					{
						gcmd<std::string> pro;
						pro.id = id;
						pro.m_operator = "query_responce";
						pro.m_data = "";
						int64_t lid = 0;
						if (aos.read("data", lid) == false)
							return;
						if (ngl::db_data<TDBTAB>::find(lid) == nullptr)
							db_manage::select<TDBTAB>::fun(actor_dbpool::get(athread), lid);
						protobuf_data<TDBTAB> m_savetemp;
						m_savetemp.m_isbinary = false;
						m_savetemp.m_data = std::make_shared<TDBTAB>();
						if (ngl::db_data<TDBTAB>::get(lid, *m_savetemp.m_data.get()))
						{
							char lbuff[10240] = { 0 };
							ngl::serialize lserialize(lbuff, 10240);
							if (lserialize.push(m_savetemp))
							{
								pro.m_data = lbuff;
							}
						}
					});
				handle_cmd::push("change", [this](int athread, int id, ngl::json_read& aos)
					{
						gcmd<bool> pro;
						pro.id = id;
						pro.m_operator = "change_responce";
						pro.m_data = false;

						std::string ljson;
						if (aos.read("data", ljson) == false)
							return;
						protobuf_data<TDBTAB> ldata;
						ldata.m_isbinary = false;
						ngl::unserialize lunser(ljson.c_str(), ljson.size() + 1);
						if (!lunser.pop(ldata))
							return;
						int64_t lid = ldata.m_data->m_id();
						ngl::db_data<TDBTAB>::set(lid, *ldata.m_data);
						db_manage::save<TDBTAB>::fun(actor_dbpool::get(athread), lid);
						pro.m_data = true;
					});
			}

			if (handle_cmd::function(loperator, adata.m_thread, adata.m_data->identifier(), lojson) == false)
			{
				log_error()->print("GM actor_db operator[{}] ERROR", loperator);
			}
			return true;
		}
	};

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	template <typename TDB>
	void actor_dbtab<TDBTAB_TYPE, TDBTAB>::cachelist(enum_cache_list atype, std::set<i64_actorid>& aset)
	{
		if (aset.empty())
			return;
		std::shared_ptr<np_actortime_db_cache<TDB>> pro(new np_actortime_db_cache<TDB>());
		pro->m_type = atype;
		pro->m_ls.swap(aset);

		ENUM_ACTOR ltype = nactor_type<actor_db<TDBTAB_TYPE, TDBTAB>>::type();
		i64_actorid lactorid = nguid::make(ltype, tab_self_area, nguid::none_actordataid());
		actor_base::static_send_actor(lactorid, nguid::make(), pro);
	}
}//namespace ngl
