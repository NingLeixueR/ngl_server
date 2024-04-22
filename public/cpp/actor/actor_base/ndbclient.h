#pragma once

#include "actor_manage.h"
#include "actor_db.h"
#include "actor.h"
#include "ttab_dbload.h"
#include "nactortype.h"
#include "nregister.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "net.h"

namespace ngl
{
	class ndbclient_base
	{
	protected:
		pbdb::ENUM_DB m_type;
		ndbclient_base(pbdb::ENUM_DB atype) :
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

		pbdb::ENUM_DB type()
		{
			return m_type;
		}
	};

	class data_modified_base
	{
	protected:
		bool m_ischange = false;
	public:
		bool is_modified()const
		{
			return m_ischange;
		}

		void modified()
		{
			m_ischange = true;
		}

		void clear_modified()
		{
			m_ischange = false;
		}
	};

	template <typename TDBTAB>
	struct data_modified : data_modified_base
	{
	private:
		TDBTAB m_data;
		TDBTAB* m_pdata = nullptr;
	public:
		data_modified(TDBTAB* adata):
			m_pdata(adata)
		{}

		data_modified(const TDBTAB& adata) :
			m_data(adata)
		{}

		data_modified():
			m_pdata(nullptr)
		{}

		void set(const TDBTAB& adata, bool achange = false)
		{
			m_data = adata;
			m_pdata = nullptr;
			if (achange)
				m_ischange = true;
		}

		TDBTAB& get(bool achange = true)
		{
			if (achange)
				m_ischange = true;
			return m_pdata == nullptr ? m_data : *m_pdata;
		}

		const TDBTAB& getconst()const
		{
			return m_pdata == nullptr ? m_data : *m_pdata;
		}
	};

	template <
		EPROTOCOL_TYPE PROTYPE, 
		pbdb::ENUM_DB TDBTAB_TYPE, 
		typename TDBTAB
	>
	class actor_db;

	template <
		EPROTOCOL_TYPE PROTYPE, 
		pbdb::ENUM_DB DBTYPE, 
		typename TDBTAB, 
		typename TACTOR
	>
	class ndbclient : public ndbclient_base
	{
		tab_dbload* m_tab;
	public:
		virtual void load()
		{
			Try
			{
				tab_servers* tab = ttab_servers::tab();
				// 向actor client 设置连接后事件
				std::shared_ptr<np_actornode_connect_task> pro(new np_actornode_connect_task
					{
						.m_serverid = tab->m_db,
						.m_fun = std::bind(&ndbclient<PROTYPE, DBTYPE, TDBTAB, TACTOR>::loaddb, this, m_id),
					});
				nguid lclientguid = nguid::make(ACTOR_CLIENT, tab_self_area, nconfig::m_nodeid);
				actor_base::static_send_actor(lclientguid, m_actor->guid(), pro);
			}Catch
		}
	private:
		void init_load()
		{
			load();
		}

		i32_actordataid dbnodeid()
		{
			tab_servers* tab = ttab_servers::tab();
			return tab->m_db;
		}

		inline i64_actorid dbguid()
		{
			ENUM_ACTOR ltype = nactor_type<actor_db<PROTYPE, DBTYPE, TDBTAB>>::type();
			return nguid::make(ltype, tab_self_area, nguid::none_actordataid());
		}

		void loaddb(const nguid& aid)
		{
			np_actordb_load<PROTYPE, DBTYPE, TDBTAB> ldata;
			ldata.m_id = aid;
			nets::sendbyserver(dbnodeid(), ldata, dbguid(), m_actor->id_guid());

			std::string lname;
			LogLocalError("actor_dbclient loaddb [%] [%]"
				, tools::type_name<ndbclient<PROTYPE, DBTYPE, TDBTAB, TACTOR>>(lname)
				, aid
			)
		}

		nguid										m_id;
		std::map<nguid, data_modified<TDBTAB>>		m_data;
		data_modified<TDBTAB>*						m_dbdata;
		bool										m_load;
		actor_manage_dbclient*						m_manage_dbclient;
		actor_base*									m_actor;
		std::vector<int64_t>						m_dellist;
	public:
		ndbclient():
			m_id(nguid::make()),
			m_load(false),
			m_dbdata(nullptr),
			m_tab(nullptr),
			ndbclient_base(DBTYPE),
			m_actor(nullptr),
			m_manage_dbclient(nullptr)
		{}

		virtual void create(const nguid& aid)
		{
			m_dbdata = &m_data[aid];
			m_dbdata->get().set_m_id(aid);
			m_id = aid;
		}

		void set_id(const nguid& aid)
		{ 
			m_id = aid;
		}

		void set_actor(actor_base* aactor)
		{
			m_manage_dbclient = aactor->get_actor_manage_dbclient();
			m_actor = aactor;
		}

		std::map<nguid, data_modified<TDBTAB>>& get_data()
		{ 
			return m_data; 
		}

		data_modified<TDBTAB>* get_data(const nguid& aid)
		{
			if (aid == m_id && m_id != -1)
				return m_dbdata;
			auto itor = m_data.find(aid);
			if (itor == m_data.end())
				return nullptr;
			return &itor->second;
		}

		data_modified<TDBTAB>* get_dbdata()
		{
			return m_dbdata;
		}

		void init(actor_manage_dbclient* amdb, actor_base* aactor, const nguid& aid)
		{
			Try
			{
				m_actor = aactor;
				m_manage_dbclient = amdb;
				m_id = aid;
				m_load = false;
				Assert(m_tab = ttab_dbload::get_tabdb<TDBTAB>(); m_tab != nullptr);

				static bool m_register = false;
				if (m_register == false)
				{
					m_register = true;
					actor::template register_db<PROTYPE, TACTOR, DBTYPE, TDBTAB>(nullptr);
				}
				init_load();
			}Catch
		}

		virtual bool isload()	
		{ 
			return m_load; 
		}

		void savedb_all()		
		{ 
			savedb(-1); 
		}

		virtual void savedb()			
		{ 
			savedb(m_id); 
		}

		void savedb(const nguid& aid)
		{
			np_actordb_save<PROTYPE, DBTYPE, TDBTAB> pro;
			std::list<data_modified<TDBTAB>*> lclearlist;
			if (aid != (int64_t)-1)
			{
				data_modified<TDBTAB>* lp = nullptr;
				if (aid == m_id)
				{
					lp = m_dbdata;
				}
				else
				{
					auto itor = m_data.find(aid);
					if (itor == m_data.end())
						return;
					lp = &itor->second;
				}
				if (lp->is_modified())
				{
					lclearlist.push_back(lp);
					pro.add(lp->getconst().m_id(), lp->getconst());
				}			
			}
			else
			{
				for (auto& [_, data] : m_data)
				{
					if (data.is_modified())
					{
						lclearlist.push_back(&data);
						pro.add(data.getconst().m_id(), data.getconst());
					}
				}
			}
			if (pro.empty() == false)
			{
				// ### 先序列化 再让actor_client确认位置
				i64_actorid lactorid = dbguid();
				std::shared_ptr<pack> lpack = actor_base::net_pack(pro, lactorid, m_actor->guid());
				if (lpack == nullptr)
				{
					// LogLocalError("actor_dbclient<%> actor_base::net_pack fail", TDBTAB::name())
					return;
				}
				// ### 异步发送pack
				m_actor->send_actor_pack(lactorid, lpack);
				// ### 执行清空[裁剪修改]标志位 
				for (data_modified<TDBTAB>* dataptr : lclearlist)
				{
					dataptr->clear_modified();
				}
			}
		}

		void del(const nguid& aid)
		{
			m_dellist.push_back((int64_t)aid);
			m_data.erase((int64_t)aid);
		}

		virtual void deldb()
		{
			np_actordb_delete<PROTYPE, DBTYPE, TDBTAB> pro;
			if (m_dellist.empty())
				return;
			pro.m_data.swap(m_dellist);
			if (pro.m_data.empty() == false)
			{
				// ### 先序列化 再让actor_client确认位置
				i64_actorid lactorid = dbguid();
				std::shared_ptr<pack> lpack = actor_base::net_pack(pro, lactorid, m_actor->guid());
				if (lpack == nullptr)
				{
					//LogLocalError("actor_dbclient<%> actor_base::net_pack fail", TDBTAB::name())
					return;
				}
				// ### 异步发送pack
				m_actor->send_actor_pack(lactorid, lpack);
			}
		}
	public:
		const TDBTAB* set(const nguid& aid, const TDBTAB& adbtab)
		{
			m_data[aid] = adbtab;
			if (aid == m_id)
				m_dbdata = &m_data[aid];
			return &m_data[aid];
		}

		data_modified<TDBTAB>* add(const nguid& aid, const TDBTAB& adbtab)
		{
			if (m_data.find(aid) != m_data.end())
				return nullptr;
			data_modified<TDBTAB>* lpdata = &m_data[aid];
			lpdata->set(adbtab, true);
			savedb(aid);
			if(aid == m_id)
				m_dbdata = lpdata;
			return lpdata;
		}

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

		bool loadfinish(std::map<nguid, TDBTAB>& adata, bool aisover)
		{
			for (auto& item : adata)
			{
				m_data[item.first].set(item.second);
			}

			auto itor = m_data.find(m_id);
			if (itor != m_data.end())
				m_dbdata = &itor->second;
			if (aisover)
			{
				m_load = true;
				m_manage_dbclient->on_load_finish(true);
			}
			return true;
		}

		bool handle(message<np_actordb_load_response<PROTYPE, DBTYPE, TDBTAB>>& adata)
		{
			Try
			{
				if (!adata.m_data->m_stat)
				{//加载失败  数据库中没有数据
					return loadfinish();
				}
				std::string lname;
				LogLocalError("db load respones:[%] recv_over[%]"
					, tools::type_name<np_actordb_load_response<PROTYPE, DBTYPE, TDBTAB>>(lname)
					, adata.m_data->m_over? "true":"false"
				)
				loadfinish(adata.m_data->data(), adata.m_data->m_over);
			}Catch
			return true;
		}

		virtual void clear_modified()
		{
			for (auto& [_, data] : m_data)
			{
				data.clear_modified();
			}
		}
	};

	class actor_manage_dbclient
	{
		using tmap_dbclient = std::map<pbdb::ENUM_DB, ndbclient_base*>;
		actor_base*						m_actor;
		tmap_dbclient					m_typedbclientmap;
		tmap_dbclient					m_dbclientmap;			//已经加载完的
		std::function<void(bool)>		m_fun;					//bool db数据库是否有该数据
		bool							m_finish;
	public:
		actor_manage_dbclient(actor_base* aactor) :
			m_actor(aactor),
			m_finish(false)
		{}

		void add(ndbclient_base* adbclient, const nguid& aid)
		{
			Try
			{
				Assert(m_typedbclientmap.find(adbclient->type()) == m_typedbclientmap.end())
				m_typedbclientmap.insert(std::make_pair(adbclient->type(), adbclient));
				init(adbclient, m_actor,  aid);
			}Catch		
		}

		void set_loadfinish_function(const std::function<void(bool)>& afun)
		{
			m_fun = afun;
		}

		void init(ndbclient_base* adbclient, actor_base* aactor, const nguid& aid)
		{
			Try
			{
				adbclient->init(this, aactor, aid);
			}Catch;
		}

		bool on_load_finish(bool adbishave)
		{
			if (m_typedbclientmap.empty())
			{
				LogLocalError("on_load_finish m_typedbclientmap.empty()")
				return false;
			}
				
			for (auto itor = m_typedbclientmap.begin(); itor != m_typedbclientmap.end();)
			{
				if (!itor->second->isload())
				{
					++itor;
					continue;
				}
				//itor->second->remove_rfun(m_actor);
				m_dbclientmap.insert(std::make_pair(itor->first, itor->second));
				itor = m_typedbclientmap.erase(itor);
			}
			if (!m_typedbclientmap.empty())
			{
				LogLocalError("on_load_finish !m_typedbclientmap.empty()")
				return false;
			}
				
			m_actor->db_component_init_data();
			// 1、将数据修改为[裁剪修改]
			for (auto& [_, data] : m_dbclientmap)
			{
				data->clear_modified();
			}
			// 2、做一些初始化之类的工作,并且需要的话将其发送给客户端
			m_fun(adbishave);
			return true;
		}

		bool isloadfinish()
		{
			return m_typedbclientmap.empty();
		}

		template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
		ndbclient<PROTYPE, ENUM, TDATA, TACTOR>* data(bool aloadfinish)
		{
			ndbclient_base** lp = ngl::tools::findmap<pbdb::ENUM_DB, ndbclient_base*>(aloadfinish? m_dbclientmap : m_typedbclientmap, ENUM);
			if (lp == nullptr)
				return nullptr;
			return (ndbclient<PROTYPE, ENUM, TDATA, TACTOR>*)(*lp);
		}

	private:
		void foreach_function(const std::function<void(ndbclient_base*)>& afun)
		{
			for (auto itor = m_dbclientmap.begin();
				itor != m_dbclientmap.end(); ++itor)
			{
				afun(itor->second);
			}
		}
	public:

		void save()
		{
			foreach_function([](ndbclient_base* ap)
				{
					ap->savedb();
				});
		}

		void del()
		{
			foreach_function([](ndbclient_base* ap)
				{
					ap->deldb();
				});
		}
	};
}

namespace ngl
{
	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
	bool actor_base::handle(message<np_actordb_load_response<PROTYPE, DBTYPE, TDBTAB>>& adata)
	{
		std::unique_ptr<actor_manage_dbclient>& mdbclient = get_actor_manage_dbclient();
		if (mdbclient == nullptr)
		{
			return false;
		}
		ndbclient<PROTYPE, DBTYPE, TDBTAB, TACTOR>* lp = mdbclient->data<PROTYPE, DBTYPE, TDBTAB, TACTOR>(false);
		if (lp == nullptr)
		{
			return false;
		}
		return lp->handle(adata);
	}
}//namespace ngl
