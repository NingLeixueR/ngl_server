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

#include "actor_manage.h"
#include "ttab_dbload.h"
#include "ndbclient.h"
#include "nregister.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_cache.h"
#include "db_data.h"
#include "db_pool.h"
#include "gcmd.h"
#include "cmd.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	class ndbtab
	{
		ndbtab() = delete;
		ndbtab(const ndbtab&) = delete;
		ndbtab& operator=(const ndbtab&) = delete;

		static tab_dbload*		m_tab;				// 数据库加载策略
		static db_cache			m_cache_save;		// 数据保存队列
		static db_cache			m_cache_del;		// 数据删除队列

		enum
		{
			esend_max_cout = 10,
		};

		static void cachelist(enum_cache_list atype, std::set<i64_actorid>& aset);
	public:
		// # 初始化
		static void init()
		{
			m_tab = ttab_dbload::instance().get_tabdb<TDBTAB>();

			if (m_tab == nullptr)
			{
				tools::no_core_dump();
				return;
			}

			// # 设置数据保存/数据删除队列
			m_cache_save.set_cachefun(std::bind_front(&cachelist, enum_clist_save), m_tab->m_dbcacheintervalms);
			m_cache_del.set_cachefun(std::bind_front(&cachelist, enum_clist_del), m_tab->m_dbcacheintervalms);

			if (m_tab->m_isloadall)
			{// 加载全部数据
				db_manage::select<TDBTAB>(db_pool::instance().get(0));
			}
			else
			{// 加载全部id 防止内存穿透
				db_manage::select<TDBTAB>(db_pool::instance().get(0), db_data<TDBTAB>::id_index());
			}
		}

		// # 加载表中的所有数据
		static void load(const pack* apack)
		{
			if (!m_tab->m_isloadall)
			{
				tools::no_core_dump();
				return;
			}
			if (!m_tab->m_network)
			{
				return;
			}
			int lsendmaxcount = m_tab->m_sendmaxcount;
			if (lsendmaxcount <= 0)
			{
				lsendmaxcount = esend_max_cout;
			}
			i64_actorid lrequestactor = apack->m_head.get_request_actor();
			np_actordb_load_response<TDBTAB_TYPE, TDBTAB> pro;
			pro.m_id = nguid::make();
			pro.m_over = false;
			pro.m_stat = enum_dbstat_success;
			ngl::db_data<TDBTAB>::foreach_index([lrequestactor, lsendmaxcount, apack, &pro](int aindex, TDBTAB& atab)
				{
					nguid lguid(atab.mid());
					pro.m_data.insert(std::make_pair(lguid, atab));
					if (aindex % lsendmaxcount == 0)
					{
						nets::send(apack->m_id, pro, lrequestactor, nguid::make());
						pro = np_actordb_load_response<TDBTAB_TYPE, TDBTAB>();
						pro.m_stat = enum_dbstat_success;
						pro.m_over = false;
					}
				}
			);
			pro.m_over = true;
			nets::send(apack->m_id, pro, lrequestactor, nguid::make());
			log_info()->print("loadall[{}]", tools::type_name<TDBTAB>());
		}

		// # 加载表中的指定数据
		static void load(i32_threadid athreadid, int64_t aid)
		{
			if (aid == -1)
			{
				return;
			}
			if (ngl::db_data<TDBTAB>::data_stat(aid) == ngl::db_data<TDBTAB>::edbdata_notload)
			{
				db_manage::select<TDBTAB>(db_pool::instance().get(athreadid), aid);
			}
		}

		// # 加载数据 ：同步方式
		static void load(i32_threadid athreadid, const pack* apack, const np_actordb_load<TDBTAB_TYPE, TDBTAB>& adata)
		{
			if (!m_tab->m_network)
			{
				return;
			}
			i64_actorid lid = adata.m_id.id();			
			if (lid == nguid::none_actordataid())
			{
				//加载全部数据
				load(apack);
			}
			else
			{
				np_actordb_load_response<TDBTAB_TYPE, TDBTAB> pro; 
				pro.m_id = lid;
				pro.m_over = true;

				if(db_data<TDBTAB>::data_stat(lid) == db_data<TDBTAB>::edbdata_notdata)
				{
					log_error()->print("load fail notdata {}:{}", tools::type_name<ndbtab<TDBTAB_TYPE, TDBTAB>>(), nguid(lid));
					TDBTAB ltemp;
					ltemp.set_mid(lid);
					db_data<TDBTAB>::set(lid, ltemp);
					pro.m_stat = enum_dbstat_create;
					m_cache_save.push(lid);
				}
				else
				{
					load(athreadid, lid);
					pro.m_stat = enum_dbstat_success;
				}
				
				TDBTAB* ldata = ngl::db_data<TDBTAB>::find(lid);
				if (ldata != nullptr)
				{
					(pro.m_data)[adata.m_id] = *ldata;
				}

				i64_actorid lrequestactor = apack->m_head.get_request_actor();
				nets::send(apack->m_id, pro, lrequestactor, nguid::make());

				log_error()->print("load finish {}:{}", tools::type_name<np_actordb_load<TDBTAB_TYPE, TDBTAB>>(), nguid(lrequestactor));
			}
		}

		// # 异步保存数据  将需要保存的数据添加到缓存保存队列
		static void save(i32_threadid, const TDBTAB& adata)
		{
			int64_t lid = adata.mid();
			ngl::db_data<TDBTAB>::set(lid, adata);
			m_cache_save.push(lid);
		}

		// # 异步删除数据  将需要删除的数据添加到缓存保存队列
		static void del(i32_threadid, i64_actorid aid)
		{
			ngl::db_data<TDBTAB>::remove(aid);
			m_cache_del.push(aid);
		}

		// # 异步删除一组数据
		static void del(i32_threadid, const std::vector<i64_actorid>& aids)
		{
			ngl::db_data<TDBTAB>::remove(aids);
			m_cache_del.push(aids);
		}

		// # 数据保存
		static void save(i32_threadid athreadid, const pack*, const np_actordb_save<TDBTAB_TYPE, TDBTAB>& adata)
		{
			const std::map<nguid, TDBTAB>& lmap = adata.m_data;
			for (const std::pair<const nguid, TDBTAB>& item : lmap)
			{
				save(athreadid, item.second);
			}
		}
	};

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	tab_dbload* ndbtab<TDBTAB_TYPE, TDBTAB>::m_tab = nullptr;

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	db_cache ndbtab<TDBTAB_TYPE, TDBTAB>::m_cache_save;

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	db_cache ndbtab<TDBTAB_TYPE, TDBTAB>::m_cache_del;

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	class actor_db :
		public actor
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
						.m_area = tab_self_area,
					},
					.m_weight = 0x7fffffff,
				})
		{
			ndbtab<TDBTAB_TYPE, TDBTAB>::init();
		}
	public:
		friend class actor_instance<tactor_db>;
		static tactor_db& instance()
		{
			return actor_instance<tactor_db>::instance();
		}

		~actor_db() final = default;

		static void nregister()
		{
			register_handle<tactor_db>::template func<
				np_actordb_load<TDBTAB_TYPE, TDBTAB>
				, np_actordb_delete<TDBTAB_TYPE, TDBTAB>
				, np_actordb_save<TDBTAB_TYPE, TDBTAB>
				, np_actortime_db_cache<TDBTAB>
				, mforward<np_gm>
			>(e_ready_all);
		}

		// # 同步:加载数据
		bool handle(const message<np_actordb_load<TDBTAB_TYPE, TDBTAB>>& adata)
		{
			std::string lname = tools::type_name<TDBTAB>();
			log_error()->print("load: np_actordb_load<{}> id:{}", lname, adata.get_data()->m_id);
			ndbtab<TDBTAB_TYPE, TDBTAB>::load(adata.thread(), adata.get_pack(), *adata.get_data());
			return true;
		}

		// # 异步:保存数据
		bool handle(const message<np_actordb_save<TDBTAB_TYPE, TDBTAB>>& adata)
		{
			ndbtab<TDBTAB_TYPE, TDBTAB>::save(adata.thread(), adata.get_pack(), *adata.get_data());
			return true;
		}

		// # 异步:删除数据
		bool handle(const message<np_actordb_delete<TDBTAB_TYPE, TDBTAB>>& adata)
		{
			ndbtab<TDBTAB_TYPE, TDBTAB>::del(adata.thread(), adata.get_data()->m_data);
			return true;
		}

		// # ACTOR_TIMER_DB_CACHE, db cache list  保存缓存列表
		bool handle(const message<np_actortime_db_cache<TDBTAB>>& adata)
		{
			enum_cache_list ltype = adata.get_data()->m_type;
			for (i64_actorid id : adata.get_data()->m_ls)
			{
				switch (ltype)
				{
				case enum_clist_save:
				{
					if (ngl::db_data<TDBTAB>::find(id) != nullptr)
					{
						db_manage::save<TDBTAB>(db_pool::instance().get(adata.thread()), id);
					}
				}
				break;
				case enum_clist_del:
				{
					ngl::db_data<TDBTAB>::remove(id);
					db_manage::del<TDBTAB>(db_pool::instance().get(adata.thread()), id);
				}
				break;
				default:
					break;
				}
			}
			return true;
		}

		// # 支持gm操作
		using handle_cmd = cmd<tactor_db, std::string, int, int, ngl::njread&>;
		bool handle(const message<mforward<np_gm>>& adata)
		{
			const mforward<np_gm>& parm = *adata.get_data();
			ngl::njread lojson(parm.data()->m_json.c_str());

			std::string loperator;
			if (!njson::read(lojson, "operator", loperator))
			{
				return true;
			}

			if (handle_cmd::empty())
			{
				handle_cmd::add("query") = [](int athread, int id, ngl::njread& aos)
					{
						gcmd<std::string> pro(id, "query");
						int64_t lid = 0;
						if (!njson::read(aos, "data", lid))
						{
							return;
						}
						db* ldb = db_pool::instance().get(athread);
						if (ldb == nullptr)
						{
							return;
						}
						if (ngl::db_data<TDBTAB>::find(lid) == nullptr)
						{
							db_manage::select<TDBTAB>(ldb, lid);
						}
						TDBTAB* ldata = ngl::db_data<TDBTAB>::find(lid);
						if (ldata == nullptr)
						{
							return;
						}
						db_manage::serialize<TDBTAB>(ldb, false, *ldata);
						pro.m_data = ldb->m_malloc.buff();
					};

				struct query_page
				{
					int32_t m_everypagecount;
					int32_t m_page;

					dprotocol(query_page, m_everypagecount, m_page)
				};
				handle_cmd::add("queryall") = [](int athread, int id, ngl::njread& aos)
					{
						gcmd<std::vector<std::string>> pro(id, "queryall");
						query_page lpage;
						if (!njson::read(aos, "data", lpage))
						{
							return;
						}
						int32_t lbegindex = lpage.m_everypagecount * (lpage.m_page - 1);
						int32_t lendindex = lbegindex + lpage.m_everypagecount;
						db* ldb = db_pool::instance().get(athread);
						if (ldb == nullptr)
						{
							return;
						}
						ngl::db_data<TDBTAB>::foreach_index(lbegindex, lendindex, [&pro, ldb](int32_t aindex, TDBTAB& aitem)
							{
								db_manage::serialize<TDBTAB>(ldb, false, aitem);
								pro.m_data.push_back(ldb->m_malloc.buff());
							});
					};

				handle_cmd::add("change") = [](int athread, int id, ngl::njread& aos)
					{
						gcmd<bool> pro(id, "change", false);
						std::string ljson;
						if (!njson::read(aos, "data", ljson))
						{
							return;
						}
						db* ldb = db_pool::instance().get(athread);
						if (ldb == nullptr)
						{
							return;
						}
						TDBTAB ldata;
						if (!db_manage::unserialize<TDBTAB>(ldb, false, ldata, ljson.c_str(), ljson.size()))
						{
							return;
						}
						ngl::db_data<TDBTAB>::set(ldata.mid(), ldata);
						db_manage::save<TDBTAB>(ldb, ldata.mid());
						pro.m_data = true;
					};
			}
			if (handle_cmd::function(loperator, adata.thread(), (int32_t)parm.identifier(), lojson) == false)
			{
				log_error()->print("GM actor_db operator[{}] ERROR", loperator);
			}
			return true;
		}
	};

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	void ndbtab<TDBTAB_TYPE, TDBTAB>::cachelist(enum_cache_list atype, std::set<i64_actorid>& aset)
	{
		if (aset.empty())
		{
			return;
		}
		auto pro = std::make_shared<np_actortime_db_cache<TDBTAB>>();
		pro->m_type = atype;
		pro->m_ls.swap(aset);
		ENUM_ACTOR ltype = nactor_type<actor_db<TDBTAB_TYPE, TDBTAB>>::type();
		i64_actorid lactorid = nguid::make(ltype, tab_self_area, nguid::none_actordataid());
		actor::send_actor(lactorid, nguid::make(), pro);
	}
}//namespace ngl