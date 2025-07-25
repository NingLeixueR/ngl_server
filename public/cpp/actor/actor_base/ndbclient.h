#pragma once

#include "actor_manage.h"
#include "ttab_dbload.h"
#include "nactortype.h"
#include "nregister.h"
#include "db_manage.h"
#include "actor_db.h"
#include "db_data.h"
#include "db_pool.h"
#include "actor.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class ndbclient_base
	{
	protected:
		pbdb::ENUM_DB m_type;
		explicit ndbclient_base(pbdb::ENUM_DB atype) :
			m_type(atype)
		{}
	public:
		virtual bool isload()	= 0;
		virtual void load()		= 0;
		virtual void savedb()	= 0;
		virtual void deldb()	= 0;
		virtual void create(const nguid& aid) = 0;
		virtual void init(actor_manage_dbclient* amdb, actor_base* aactor, const nguid& aid) = 0;
		virtual void clear_modified() = 0;
		virtual void nscript_push_data() = 0;

		pbdb::ENUM_DB type()const
		{
			return m_type;
		}
	};

	template <typename TDBTAB>
	class nmodified
	{
	private:
		static std::set<i64_actorid> m_modified;
	public:
		static std::set<i64_actorid>& which_modified()
		{
			return m_modified;
		}

		static bool is_modified(i64_actorid aidentifier)
		{
			return m_modified.find(aidentifier) != m_modified.end();
		}

		// # 设置为修改状态
		static void modified(i64_actorid aidentifier)
		{
			m_modified.insert(aidentifier);
		}

		// # 清空修改状态
		static void clear_modified(i64_actorid aidentifier)
		{
			m_modified.erase(aidentifier);
		}
	};

	template <typename TDBTAB>
	std::set<i64_actorid> nmodified<TDBTAB>::m_modified;


	template <typename TDBTAB>
	struct data_modified 
	{
	private:
		data_modified(const data_modified&) = delete;
		data_modified& operator=(const data_modified&) = delete;

		mutable TDBTAB m_data;
		mutable TDBTAB* m_pdata = nullptr;
		actor_base* m_actor = nullptr;
	public:
		data_modified() = default;

		inline i64_actorid identifier()const
		{
			TDBTAB& ldata = m_pdata == nullptr ? m_data : *m_pdata;
			return ldata.mid();
		}

		inline bool is_modified()
		{
			return nmodified<TDBTAB>::is_modified(identifier());
		}

		// # 设置为修改状态
		inline void modified()const
		{
			nmodified<TDBTAB>::modified(identifier());
		}

		// # 清空修改状态
		inline void clear_modified()const
		{
			nmodified<TDBTAB>::clear_modified(identifier());
		}

		void set(actor_base* aactor, const TDBTAB& adata, bool achange = false)
		{
			m_actor = aactor;
			m_data = adata;
			m_pdata = nullptr;
			if (achange)
			{
				modified();
			}
		}

		// # get与getconst获取数据前先 "检查脚本语言中的备份是否被修改"
		TDBTAB& get(bool achange = true, bool anscript = true)
		{
			TDBTAB& ldata = m_pdata == nullptr ? m_data : *m_pdata;
			if (achange)
			{
				modified();
			}
			// # 检查脚本语言中的备份是否被修改
			if (anscript && m_actor != nullptr && m_actor->nscript_check_outdata(ldata.mid(), ldata))
			{
				modified();
			}
			return ldata;
		}

		const TDBTAB& getconst(bool anscript = true)const
		{
			TDBTAB& ldata = m_pdata == nullptr ? m_data : *m_pdata;
			// # 检查脚本语言中的备份是否被修改
			if (anscript && m_actor != nullptr && m_actor->nscript_check_outdata(ldata.mid(), ldata))
			{
				modified();
			}
			return ldata;
		}
	};

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	class actor_db;

	template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
	class ndbclient : 
		public ndbclient_base, tools_log
	{
		ndbclient(const ndbclient&) = delete;
		ndbclient& operator=(const ndbclient&) = delete;

		using type_ndbclient = ndbclient<DBTYPE, TDBTAB, TACTOR>;
		
		tab_dbload* m_tab = nullptr;
	public:
		// # 向actor_client设置连接后事件
		// # 当与db服务器发生连接时触发加载数据事件
		void load() final
		{
			const tab_servers* tab = ttab_servers::instance().tab();
			if (tab == nullptr)
			{
				tools::no_core_dump();
				return;
			}
			auto pro = std::make_shared<np_actornode_connect_task>();
			pro->m_serverid = tab->m_db;
			pro->m_fun = std::bind_front(&type_ndbclient::loaddb, this, m_id);
			nguid lclientguid = actor_client::actorid();
			actor::send_actor(lclientguid, m_actor->guid(), pro);
		}
	private:
		// # 加载数据
		void init_load()
		{
			std::string& lname = tools::type_name<type_ndbclient>();
			log_error()->print("ndbclient init_load [{}]", lname);
			load();
		}

		// # db结点的id
		i32_actordataid dbnodeid()
		{
			const tab_servers* tab = ttab_servers::instance().tab();
			return tab->m_db;
		}

		// # 获取db actor的guid
		inline i64_actorid dbguid()
		{
			ENUM_ACTOR ltype = nactor_type<actor_db<DBTYPE, TDBTAB>>::type();
			return nguid::make(ltype, tab_self_area, nguid::none_actordataid());
		}

		// # 加载数据
		void loaddb(const nguid& aid)
		{
			np_actordb_load<DBTYPE, TDBTAB> ldata;
			ldata.m_id = aid;
			nets::sendbyserver(dbnodeid(), ldata, dbguid(), m_actor->id_guid());
			std::string& lname = tools::type_name<type_ndbclient>();
			log_error()->print("ndbclient loaddb [{}] [{}]", lname, aid);
		}

		nguid										m_id = nguid::make();
		std::map<nguid, data_modified<TDBTAB>>		m_data;
		data_modified<TDBTAB>*						m_dbdata = nullptr;
		bool										m_load = false;
		actor_manage_dbclient*						m_manage_dbclient = nullptr;
		actor_base*									m_actor = nullptr;
		std::vector<int64_t>						m_dellist;									// 删除列表
		std::string									m_name = tools::type_name<type_ndbclient>();// 主要调试需要知道TACTOR的名字
	public:
		ndbclient():
			ndbclient_base(DBTYPE)
		{
		}

		void create(const nguid& aid) final
		{
			m_dbdata = &m_data[aid];
			m_dbdata->get().set_mid(aid);
			m_id = aid;
		}

		void set_id(const nguid& aid)
		{ 
			m_id = aid;
		}

		// # 设置持有此数据的actor
		void set_actor(actor_base* aactor)
		{
			m_manage_dbclient = aactor->get_actor_manage_dbclient();
			m_actor = aactor;
			set_logactor(aactor);
		}

		// # 获取数据
		std::map<nguid, data_modified<TDBTAB>>& get_data()
		{ 
			return m_data; 
		}

		// # 获取nguid数据
		data_modified<TDBTAB>* get_data(const nguid& aid)
		{
			if (aid == m_id && m_id != nguid::make())
			{
				return m_dbdata;
			}
			return tools::findmap(m_data, aid);
		}

		// # 获取数据
		data_modified<TDBTAB>* get_dbdata()
		{
			return m_dbdata;
		}

		void init(actor_manage_dbclient* amdb, actor_base* aactor, const nguid& aid) final
		{
			m_actor				= aactor;
			m_manage_dbclient	= amdb;
			m_id				= aid;
			m_load				= false;

			set_logactor(aactor);

			m_tab = ttab_dbload::instance().get_tabdb<TDBTAB>();
			if (m_tab == nullptr)
			{
				tools::no_core_dump();
				return;
			}

			static std::atomic<bool> lregister = true;
			if (lregister.exchange(false))
			{
				actor::template register_db<TACTOR, DBTYPE, TDBTAB>(nullptr);
			}
			init_load();
		}

		bool isload() final	
		{ 
			return m_load; 
		}

		void savedb_all()		
		{ 
			savedb(nguid::make()); 
		}

		void savedb()final
		{ 
			savedb(m_id); 
		}

		void savedb(const nguid& aid)
		{
			if (aid == nguid::make())
			{
				m_actor->nscript_check_outdata(m_data);
				std::vector<int64_t> ldelvec;
				if (m_actor->nscript_check_outdata_del<TDBTAB>(ldelvec))
				{
					del(ldelvec);
				}
			}
			else
			{
				if (m_actor->nscript_check_outdata_del<TDBTAB>((int64_t)aid))
				{
					del(aid);
				}
				else
				{
					data_modified<TDBTAB>* lpdata = tools::findmap(m_data, aid);
					lpdata->getconst();
				}
			}

			np_actordb_save<DBTYPE, TDBTAB> pro;
			data_modified<TDBTAB>* lpdata = nullptr;
			if (aid != nguid::make())
			{
				if (aid == m_id)
				{
					lpdata = m_dbdata;
				}
				else
				{
					lpdata = tools::findmap(m_data, aid);
				}
				if (lpdata != nullptr && lpdata->is_modified())
				{
					pro.add(aid, lpdata->getconst(false));
					// # 清空标志位 
					lpdata->clear_modified();
				}			
			}
			else
			{
				std::set<i64_actorid>& lmodified = nmodified<TDBTAB>::which_modified();
				for (i64_actorid lactorid : lmodified)
				{
					lpdata = tools::findmap(m_data, nguid(lactorid));
					pro.add(lactorid, lpdata->getconst(false));
				}
				// # 清空标志位 
				lmodified.clear();
			}
			if (!pro.empty())
			{
				log_error()->print("ndbclient<{}> save {}", m_name, aid);
				// # 先序列化 再让actor_client确认位置
				i64_actorid lactorid = dbguid();
				std::shared_ptr<pack> lpack = actor_base::net_pack(pro, lactorid, m_actor->guid());
				if (lpack == nullptr)
				{
					log_error()->print("ndbclient<{}> actor_base::net_pack fail", m_name);
					return;
				}
				// # 异步发送pack
				m_actor->send_actor_pack(lactorid, lpack);
			}
		}

		void del(const nguid& aid)
		{
			m_dellist.push_back((int64_t)aid);
			m_data.erase((int64_t)aid);
		}

		void del(std::vector<int64_t>& adelvec)
		{
			for (int64_t delval : adelvec)
			{
				m_dellist.push_back(delval);
				m_data.erase(delval);
			}
		}

		void deldb() final
		{
			np_actordb_delete<DBTYPE, TDBTAB> pro;
			if (m_dellist.empty())
			{
				return;
			}
			pro.m_data.swap(m_dellist);
			if (pro.m_data.empty() == false)
			{
				// # 先序列化 再让actor_client确认位置
				i64_actorid lactorid = dbguid();
				std::shared_ptr<pack> lpack = actor_base::net_pack(pro, lactorid, m_actor->guid());
				if (lpack == nullptr)
				{
					log_error()->print("ndbclient<{}> actor_base::net_pack fail", m_name);
					return;
				}
				// # 异步发送pack
				m_actor->send_actor_pack(lactorid, lpack);
			}
		}

		void nscript_push_data() final
		{
			if (!m_actor->nscript_using())
			{
				return;
			}
			std::map<int64_t, TDBTAB*> lmap;
			for (std::pair<const nguid, data_modified<TDBTAB>>& item : m_data)
			{
				lmap.insert(std::make_pair((int64_t)item.first, &item.second.get(false, false)));
			}
			m_actor->nscript_proto_push_data<TDBTAB>("db", lmap, true);
		}
	public:
		const TDBTAB* set(const nguid& aid, const TDBTAB& adbtab)
		{
			m_data[aid] = adbtab;
			if (aid == m_id)
			{
				m_dbdata = &m_data[aid];
			}
			return &m_data[aid];
		}

		// # 加载完成
		bool loadfinish()
		{
			if (m_id != (int64_t)-1)
			{
				m_dbdata = &m_data[m_id];
			}
			m_load = true;
			m_manage_dbclient->on_load_finish(false);
			return false;
		}

		bool loadfinish(const std::map<nguid, TDBTAB>& adata, bool aisover)
		{
			for (const std::pair<const nguid, TDBTAB>& lpair : adata)
			{
				m_data[lpair.first].set(m_actor, lpair.second);
			}

			auto itor = m_data.find(m_id);
			if (itor != m_data.end())
			{
				m_dbdata = &itor->second;
			}
			if (aisover)
			{
				m_load = true;
				m_manage_dbclient->on_load_finish(true);
			}
			return true;
		}

		bool handle(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata)
		{
			if (!adata.get_data()->m_stat)
			{//加载失败  数据库中没有数据
				return loadfinish();
			}
			using type_message = np_actordb_load_response<DBTYPE, TDBTAB>;
			log_error()->print(
				"db load respones:[{}] recv_over[{}]"
				, tools::type_name<type_message>()
				, adata.get_data()->m_over ? "finish" : "no finishi"
			);
			loadfinish(adata.get_data()->data(), adata.get_data()->m_over);
			return true;
		}

		void clear_modified() final
		{
			for(std::pair<const nguid, data_modified<TDBTAB>>& lpair : m_data)
			{
				lpair.second.clear_modified();
			}
		}
	};

	class actor_manage_dbclient
	{
		using tmap_dbclient = std::map<pbdb::ENUM_DB, ndbclient_base*>;
		actor_base*						m_actor = nullptr;
		tmap_dbclient					m_typedbclientmap;
		tmap_dbclient					m_dbclientmap;			//已经加载完的
		std::function<void(bool)>		m_fun;					//bool db数据库是否有该数据
	public:
		explicit actor_manage_dbclient(actor_base* aactor) :
			m_actor(aactor)
		{}

		void add(ndbclient_base* adbclient, const nguid& aid)
		{
			if (m_typedbclientmap.contains(adbclient->type()))
			{
				tools::no_core_dump();
				return;
			}
			m_typedbclientmap.insert(std::make_pair(adbclient->type(), adbclient));
			init(adbclient, m_actor, aid);
		}

		void set_loadfinish_function(const std::function<void(bool)>& afun)
		{
			m_fun = afun;
		}

		void init(ndbclient_base* adbclient, actor_base* aactor, const nguid& aid)
		{
			adbclient->init(this, aactor, aid);
		}

		bool on_load_finish(bool adbishave)
		{
			if (m_typedbclientmap.empty())
			{
				log_error()->print("on_load_finish m_typedbclientmap.empty()");
				return false;
			}
			
			for (auto itor = m_typedbclientmap.begin(); itor != m_typedbclientmap.end();)
			{
				if (!itor->second->isload())
				{
					++itor;
					continue;
				}
				m_dbclientmap.insert(std::make_pair(itor->first, itor->second));
				itor = m_typedbclientmap.erase(itor);
			}

			if (!m_typedbclientmap.empty())
			{
				log_error()->print("on_load_finish !m_typedbclientmap.empty()");
				return false;
			}

			actor_manage::instance().set_ready(m_actor->id_guid());
				
			m_actor->db_component_init_data();

			// 1、将数据修改为[裁剪修改]
			for (std::pair<const pbdb::ENUM_DB, ndbclient_base*>& lpair : m_dbclientmap)
			{
				lpair.second->clear_modified();
			}

			// 2、数据压倒脚本中
			nscript_push_data();

			// 3、做一些初始化之类的工作,并且需要的话将其发送给客户端
			// 3.1 c++内部操作
			m_fun(adbishave);
			// 3.2 通知脚本
			m_actor->nscript_db_loadfinish();
			return true;
		}

		bool isloadfinish()const
		{
			return m_typedbclientmap.empty();
		}

		template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
		ndbclient<ENUM, TDATA, TACTOR>* data(bool aloadfinish)
		{
			ndbclient_base** lp = ngl::tools::findmap<pbdb::ENUM_DB, ndbclient_base*>(
				aloadfinish? m_dbclientmap : m_typedbclientmap, ENUM
			);
			if (lp == nullptr)
			{
				return nullptr;
			}
			return (ndbclient<ENUM, TDATA, TACTOR>*)(*lp);
		}

	private:
		void foreach_function(const std::function<void(ndbclient_base*)>& afun)
		{
			for(std::pair<const pbdb::ENUM_DB, ndbclient_base*>& lpair :m_dbclientmap)
			{
				afun(lpair.second);
			}
		}
	public:
		void save()
		{
			static std::function<void(ndbclient_base*)> lfun = [](ndbclient_base* ap)
				{
					ap->savedb();
				};
			foreach_function(lfun);
		}

		void del()
		{
			static std::function<void(ndbclient_base*)> lfun = [](ndbclient_base* ap)
				{
					ap->deldb();
				};
			foreach_function(lfun);
		}

		void nscript_push_data()
		{
			static std::function<void(ndbclient_base*)> lfun = [](ndbclient_base* ap)
				{
					ap->nscript_push_data();
				};
			foreach_function(lfun);
		}
	};
}

namespace ngl
{
	template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
	bool actor_base::handle(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata)
	{
		const std::unique_ptr<actor_manage_dbclient>& mdbclient = get_actor_manage_dbclient();
		if (mdbclient == nullptr)
		{
			tools::no_core_dump();
			return true;
		}
		ndbclient<DBTYPE, TDBTAB, TACTOR>* lp = mdbclient->data<DBTYPE, TDBTAB, TACTOR>(false);
		if (lp == nullptr)
		{
			tools::no_core_dump();
			return true;
		}
		return lp->handle(adata);
	}

	template <typename T>
	bool actor_base::nscript_check_outdata(std::map<nguid, data_modified<T>>& adata)
	{
		std::string ljson;
		if (nscript_check_outdata(tools::type_name<T>().c_str(), nguid::make(), ljson))
		{
			std::map<int64_t, T> lmap;
			if (!tools::json2proto(ljson, lmap))
			{
				return false;
			}
			for (const auto& item : lmap)
			{
				adata[item.first].get(true, false) = item.second;
			}
			return true;
		}
		return false;
	}

	template <typename T>
	bool actor_base::nscript_check_outdata_del(i64_actorid aactorid)
	{
		return nscript_check_outdata_del(tools::type_name<T>().c_str(), aactorid);
	}

	template <typename T>
	bool actor_base::nscript_check_outdata_del(std::vector<i64_actorid>& avec)
	{
		return nscript_check_outdata_del(tools::type_name<T>().c_str(), avec);
	}
}//namespace ngl
