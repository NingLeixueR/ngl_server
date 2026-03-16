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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/actor_base/nmanage_dbclient.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/nactortype.h"
#include "actor/actor_base/nregister.h"
#include "actor/actor_base/actor_db.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/actor_base/nscript.h"
#include "actor/actor_base/actor.h"
#include "actor/tab/ttab_dbload.h"
#include "tools/db/sql/db_data.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	class ndbclient_base
	{
	protected:
		pbdb::ENUM_DB m_type = pbdb::ENUM_DB::ENUM_DB_FAIL;
		explicit ndbclient_base(pbdb::ENUM_DB atype) :
			m_type(atype)
		{}
	public:
		virtual bool isload()	= 0;
		virtual void load()		= 0;
		virtual void savedb()	= 0;
		virtual void deldb()	= 0;
		virtual bool create(const nguid& aid) = 0;
		virtual void init(nmanage_dbclient* amdb, actor_base* aactor, const nguid& aid) = 0;
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
		std::set<i64_actorid> m_modified;
	public:
		// # Which
		std::set<i64_actorid>& which_modified()
		{
			return m_modified;
		}

		// # Whether
		bool is_modified(i64_actorid aidentifier)const
		{
			return m_modified.contains(aidentifier);
		}

		// # Set state
		void modified(i64_actorid aidentifier)
		{
			m_modified.insert(aidentifier);
		}

		// # Clear state
		void clear_modified(i64_actorid aidentifier)
		{
			m_modified.erase(aidentifier);
		}
	};

	template <typename TDBTAB>
	struct data_modified 
	{
	private:
		data_modified(const data_modified&) = delete;
		data_modified& operator=(const data_modified&) = delete;

		mutable TDBTAB		m_data;
		mutable TDBTAB*		m_pdata		= nullptr;
		actor_base*			m_actor		= nullptr;
		nmodified<TDBTAB>*	m_modified	= nullptr;
	public:
		data_modified() = default;

		inline void init(nmodified<TDBTAB>* amodified)
		{
			m_modified = amodified;
		}

		inline void check_init()const
		{
			if (m_modified == nullptr)
			{
				tools::no_core_dump();
			}
		}

		inline i64_actorid identifier()const
		{
			check_init();
			TDBTAB& ldata = m_pdata == nullptr ? m_data : *m_pdata;
			return ldata.mid();
		}

		inline bool is_modified()
		{
			check_init();
			return m_modified->is_modified(identifier());
		}

		// # Set state
		inline void modified()const
		{
			check_init();
			m_modified->modified(identifier());
		}

		// # Clear state
		inline void clear_modified()const
		{
			check_init();
			m_modified->clear_modified(identifier());
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

		// # Getandgetconstgetdatabeforefirst " scripting languagein whether "
		TDBTAB* get(bool achange = true, bool anscript = true)
		{
			check_init();
			TDBTAB& ldata = m_pdata == nullptr ? m_data : *m_pdata;
			i64_actorid lidentifier = identifier();
			if ((achange || anscript) && (lidentifier == 0 || lidentifier == nguid::make()))
			{
				return nullptr;
			}
			if (achange)
			{
				modified();
			}
			// # Scripting languagein whether
			if (anscript && m_actor != nullptr)
			{
				if (m_actor->nscript_data_checkdel<TDBTAB>(ldata.mid()))
				{// Delete
					return nullptr;
				}
				if (m_actor->nscript_data_checkout<TDBTAB>(ldata.mid(), ldata))
				{
					modified();
				}
			}
			return &ldata;
		}

		const TDBTAB* getconst(bool anscript = true)const
		{
			check_init();
			TDBTAB& ldata = m_pdata == nullptr ? m_data : *m_pdata;
			// # Scripting languagein whether
			if (anscript && m_actor != nullptr)
			{
				if (m_actor->nscript_data_checkdel<TDBTAB>(ldata.mid()))
				{// Delete
					return nullptr;
				}
				if (m_actor->nscript_data_checkout<TDBTAB>(ldata.mid(), ldata))
				{
					modified();
				}
			}
			return &ldata;
		}
	};

	template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
	class ndbclient : 
		public ndbclient_base
	{
		ndbclient(const ndbclient&) = delete;
		ndbclient& operator=(const ndbclient&) = delete;

		using type_ndbclient = ndbclient<DBTYPE, TDBTAB, TACTOR>;

		enum
		{
			e_default_tcount = 1,		// Defaulttcount
		};
		
		tab_dbload*									m_tab = nullptr;							// Load"tableconfigdataload"
		nmodified<TDBTAB>							m_modified;									// Recordwhichdata
		nguid										m_id = nguid::make();						// Dataid(m_id == nguid::make() loadalldata)
		std::map<nguid, data_modified<TDBTAB>>		m_data;										// Data
		data_modified<TDBTAB>*						m_dbdata = nullptr;							// M_id !=nguid::make() this
		bool										m_load = false;								// Whetherloadcomplete
		nmanage_dbclient*						m_manage_dbclient = nullptr;				// Translated comment.
		actor_base*									m_actor = nullptr;							// Hostactor
		std::vector<int64_t>						m_dellist;									// Deletelist
		std::string									m_name = tools::type_name<type_ndbclient>();// Need to TACTOR
	public:
		// # Toactor_clientsetconnectionafterevent
		// # Anddbserver connection loaddataevent
		void load() final
		{
			const tab_servers* tab = ttab_servers::instance().const_tab();
			if (tab == nullptr)
			{
				tools::no_core_dump();
				return;
			}
			auto pro = std::make_shared<np_actornode_connect_task>();
			pro->m_serverid = nnodeid::nodeid(tab->m_db, 1);
			pro->m_fun = std::bind_front(&type_ndbclient::loaddb, this, m_id);
			nguid lclientguid = actor_client::actorid();
			actor::send_actor(lclientguid, m_actor->guid(), pro);
		}
	private:
		// # Loaddata
		void init_load()
		{
			log_error()->print("ndbclient init_load [{}]", m_name);
			load();
		}

		// # Dbnode id
		i32_actordataid dbnodeid()
		{
			const tab_servers* tab = ttab_servers::instance().const_tab();
			return nnodeid::nodeid(tab->m_db, e_default_tcount);
		}

		// # Getdb actor guid
		inline i64_actorid dbguid()
		{
			ENUM_ACTOR ltype = nactor_type<actor_db<DBTYPE, TDBTAB>>::type();
			return nguid::make(ltype, tab_self_area, nguid::none_actordataid());
		}

		// # Loaddata
		void loaddb(const nguid& aid)
		{
			np_actordb_load<DBTYPE, TDBTAB> ldata;
			ldata.m_id = aid;
			ntcp::instance().send_server(dbnodeid(), ldata, dbguid(), m_actor->id_guid());
			log_error()->print("ndbclient loaddb [{}] [{}]", m_name, aid);
		}
	public:
		ndbclient():
			ndbclient_base(DBTYPE)
		{
		}

		bool create(const nguid& aid) final
		{
			if (aid == nguid::make())
			{
				return false;
			}
			if (m_data.contains(aid))
			{
				return false;
			}
			m_dbdata = &m_data[aid];
			m_dbdata->init(&m_modified);
			m_dbdata->get()->set_mid(aid);
			return true;
		}

		void set_id(const nguid& aid)
		{ 
			m_id = aid;
		}

		// # Set this data actor
		void set_actor(actor_base* aactor)
		{
			m_manage_dbclient = aactor->manage_dbclient();
			m_actor = aactor;
		}

		// # Getdata
		const std::map<nguid, data_modified<TDBTAB>>& get_data()
		{ 
			return m_data; 
		}

		std::map<nguid, data_modified<TDBTAB>>& get_foreach_data()
		{
			return m_data;
		}

		// # Getnguiddata
		data_modified<TDBTAB>* get_data(const nguid& aid)
		{
			if (aid == m_id && m_id != nguid::make())
			{
				return m_dbdata;
			}
			if (m_data.contains(aid))
			{
				return &m_data[aid];
			}
			data_modified<TDBTAB>& ldata = m_data[aid];
			TDBTAB lTDBTAB;
			lTDBTAB.set_mid(aid);
			ldata.set(m_actor, lTDBTAB);
			ldata.init(&m_modified);
			return &ldata;
		}

		// # Getdata
		data_modified<TDBTAB>* get_dbdata()
		{
			return m_dbdata;
		}

		void init(nmanage_dbclient* amdb, actor_base* aactor, const nguid& aid) final
		{
			m_actor				= aactor;
			m_manage_dbclient	= amdb;
			m_id				= aid;
			m_load				= false;

			m_tab = ttab_dbload::instance().get_tabdb<TDBTAB>();
			if (m_tab == nullptr)
			{
				tools::no_core_dump();
				return;
			}

			static std::atomic lregister = true;
			if (lregister.exchange(false))
			{
				nrfun<TACTOR>::instance().template rfun<actor_base, np_actordb_load_response<DBTYPE, TDBTAB>>(
					&actor_base::template handle_db<DBTYPE, TDBTAB, TACTOR>, e_ready_null
				);
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
			data_modified<TDBTAB>* lpdata = nullptr;

			std::vector<int64_t> ldels;
			if (m_actor->nscript_data_checkdel<TDBTAB>(ldels))
			{
				for (int64_t id : ldels)
				{
					del(id);
				}
			}

			std::map<int64_t, TDBTAB> ldata;
			if (m_actor->nscript_data_checkout<TDBTAB>(ldata))
			{
				for (auto& [_actorid, _tdb] : ldata)
				{
					lpdata = tools::findmap(m_data, nguid(_actorid));
					if (lpdata == nullptr)
					{
						continue;
					}
					*lpdata->get(true, false) = _tdb;
				}
			}

			lpdata = nullptr;
			np_actordb_save<DBTYPE, TDBTAB> pro;
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
					pro.add(aid, *lpdata->getconst(false));
					// # Clearflag bits
					lpdata->clear_modified();
				}			
			}
			else
			{
				std::set<i64_actorid>& lmodified = m_modified.which_modified();
				for (i64_actorid lactorid : lmodified)
				{
					lpdata = tools::findmap(m_data, nguid(lactorid));
					pro.add(lactorid, *lpdata->getconst(false));
				}
				// # Clearflag bits
				lmodified.clear();
			}
			if (!pro.empty())
			{
				log_error()->print("ndbclient<{}> save {}", m_name, aid);
				// # Serialize first, then let actor_client confirm the position
				i64_actorid lactorid = dbguid();
				std::shared_ptr<pack> lpack = actor_base::net_pack(pro, lactorid, m_actor->guid());
				if (lpack == nullptr)
				{
					log_error()->print("ndbclient<{}> actor_base::net_pack fail", m_name);
					return;
				}
				// # Asynchronouslysendpack
				actor::send_actor(lactorid, lpack);
			}
		}

		void clear()
		{
			for (const std::pair<const nguid, data_modified<TDBTAB>>& item : m_data)
			{
				del(item.first);
			}
		}

		void del(const nguid& aid, bool ascript = false)
		{
			if (ascript)
			{
				m_actor->nscript_data_del<TDBTAB>(aid);
			}
			m_dellist.push_back((int64_t)aid);
			m_data.erase((int64_t)aid);
			if (aid == m_id)
			{
				m_dbdata = nullptr;
			}
		}

		void del(std::vector<int64_t>& adelvec, bool ascript = false)
		{
			for (int64_t dataid : adelvec)
			{
				del(dataid, ascript);
			}
		}

		void deldb() final
		{
			std::vector<int64_t> ldels;
			if (m_actor->nscript_data_checkdel<TDBTAB>(ldels))
			{
				del(ldels);
			}

			np_actordb_delete<DBTYPE, TDBTAB> pro;
			if (m_dellist.empty())
			{
				return;
			}
			pro.m_data.swap(m_dellist);
			if (pro.m_data.empty() == false)
			{
				// # Serialize first, then let actor_client confirm the position
				i64_actorid lactorid = dbguid();
				std::shared_ptr<pack> lpack = actor_base::net_pack(pro, lactorid, m_actor->guid());
				if (lpack == nullptr)
				{
					log_error()->print("ndbclient<{}> actor_base::net_pack fail", m_name);
					return;
				}
				// # Asynchronouslysendpack
				actor::send_actor(lactorid, lpack);
			}
		}

		void nscript_push_data() final
		{
			if (!m_actor->nscript_using())
			{
				return;
			}

			nscript_data_db<TDBTAB> ltemp;
			for (std::pair<const nguid, data_modified<TDBTAB>>& item : m_data)
			{
				ltemp.data.insert(std::make_pair((int64_t)item.first, item.second.get(false, false)));
			}
			m_actor->nscript_data_push("db", ltemp, true);
		}
	public:
		bool loadfinish(const std::map<nguid, TDBTAB>& adata, enum_dbstat astat, bool aloadfinish)
		{
			for (auto& [_guid, _tdb] : adata)
			{
				data_modified<TDBTAB>& ldata = m_data[_guid];
				ldata.set(m_actor, _tdb);
				ldata.init(&m_modified);
			}
			auto lpmodified = tools::findmap(m_data, m_id);
			if(lpmodified != nullptr)
			{
				m_dbdata = lpmodified;
			}
			if (aloadfinish)
			{
				m_load = true;
				m_manage_dbclient->on_load_finish(DBTYPE, astat);
			}
			return true;
		}

		bool handle(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata)
		{
			using type_message = np_actordb_load_response<DBTYPE, TDBTAB>;
			log_error()->print("db load respones:[{}] recv_over[{}]", tools::type_name<type_message>(), adata.get_data()->m_over ? "finish" : "don`t finishi");
			loadfinish(adata.get_data()->data(), adata.get_data()->m_stat, adata.get_data()->m_over);
			return true;
		}

		void clear_modified() final
		{
			for(auto& [_guid, _data] : m_data)
			{
				_data.clear_modified();
			}
		}
	};
}//namespace ngl

namespace ngl
{

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	ndbclient<ENUM, TDATA, TACTOR>* nmanage_dbclient::data(bool aloadfinish)
	{
		ndbclient_base** lp = ngl::tools::findmap(aloadfinish ? m_dbclientmap : m_typedbclientmap, ENUM);
		if (lp == nullptr)
		{
			return nullptr;
		}
		return (ndbclient<ENUM, TDATA, TACTOR>*)(*lp);
	}

	template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
	bool actor_base::handle_db(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata)
	{
		const std::unique_ptr<nmanage_dbclient>& mdbclient = manage_dbclient();
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
}//namespace ngl

#define MODIFIED_RETUAN(NAME, DATA, ...)					\
	auto NAME = (DATA).get();								\
	if(NAME == nullptr)										\
	{														\
		return __VA_ARGS__;									\
	}

#define MODIFIED_RETURN_CONST(NAME, DATA, ...)				\
	auto NAME = (DATA).getconst();							\
	if(NAME == nullptr)										\
	{														\
		return __VA_ARGS__;									\
	}

#define MODIFIED_BREAK(NAME, DATA)							\
	auto NAME = (DATA).get();								\
	if(NAME == nullptr)										\
	{														\
		break;												\
	}

#define MODIFIED_CONST(NAME, DATA)							\
	auto NAME = (DATA).getconst();							\
	if(NAME == nullptr)										\
	{														\
		break;												\
	}

#define MODIFIED_CONTINUE(NAME, DATA)						\
	auto NAME = (DATA).get();								\
	if(NAME == nullptr)										\
	{														\
		continue;											\
	}

#define MODIFIED_CONTINUE_CONST(NAME, DATA)					\
	auto NAME = (DATA).getconst();							\
	if(NAME == nullptr)										\
	{														\
		continue;											\
	}

#define MODIFIED_DUMP(NAME, DATA)							\
	auto NAME = (DATA).get();								\
	if(NAME == nullptr)										\
	{														\
		ngl::tools::tools::no_core_dump();					\
	}

#define MODIFIED_DUMP_CONST(NAME, DATA)						\
	auto NAME = (DATA).getconst();							\
	if(NAME == nullptr)										\
	{														\
		ngl::tools::tools::no_core_dump();					\
	}