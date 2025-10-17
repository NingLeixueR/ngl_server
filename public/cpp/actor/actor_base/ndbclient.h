#pragma once

#include "actor_manage.h"
#include "ttab_dbload.h"
#include "nactortype.h"
#include "nregister.h"
#include "db_manage.h"
#include "actor_db.h"
#include "nscript.h"
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
		// # ��Щ���޸�
		static std::set<i64_actorid>& which_modified()
		{
			return m_modified;
		}

		// # ����Ƿ��޸�
		static bool is_modified(i64_actorid aidentifier)
		{
			return m_modified.contains(aidentifier);
		}

		// # ����Ϊ�޸�״̬
		static void modified(i64_actorid aidentifier)
		{
			m_modified.insert(aidentifier);
		}

		// # ����޸�״̬��
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

		// # ����Ϊ�޸�״̬
		inline void modified()const
		{
			nmodified<TDBTAB>::modified(identifier());
		}

		// # ����޸�״̬
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

		// # get��getconst��ȡ����ǰ�� "���ű������еı����Ƿ��޸�"
		TDBTAB* get(bool achange = true, bool anscript = true)
		{
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
			// # ���ű������еı����Ƿ��޸�
			if (anscript && m_actor != nullptr)
			{
				if (m_actor->nscript_data_checkdel<TDBTAB>(ldata.mid()))
				{//��ɾ����
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
			TDBTAB& ldata = m_pdata == nullptr ? m_data : *m_pdata;
			// # ���ű������еı����Ƿ��޸�
			if (anscript && m_actor != nullptr)
			{
				if (m_actor->nscript_data_checkdel<TDBTAB>(ldata.mid()))
				{//��ɾ����
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

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	class actor_db;

	template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
	class ndbclient : 
		public ndbclient_base
	{
		ndbclient(const ndbclient&) = delete;
		ndbclient& operator=(const ndbclient&) = delete;

		using type_ndbclient = ndbclient<DBTYPE, TDBTAB, TACTOR>;
		
		tab_dbload* m_tab = nullptr;
	public:
		// # ��actor_client�������Ӻ��¼�
		// # ����db��������������ʱ�������������¼�
		void load() final
		{
			const tab_servers* tab = ttab_servers::instance().tab();
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
		// # ��������
		void init_load()
		{
			log_error()->print("ndbclient init_load [{}]", m_name);
			load();
		}

		// # db����id
		i32_actordataid dbnodeid()
		{
			const tab_servers* tab = ttab_servers::instance().tab();
			return nnodeid::nodeid(tab->m_db, 1);
		}

		// # ��ȡdb actor��guid
		inline i64_actorid dbguid()
		{
			ENUM_ACTOR ltype = nactor_type<actor_db<DBTYPE, TDBTAB>>::type();
			return nguid::make(ltype, tab_self_area, nguid::none_actordataid());
		}

		// # ��������
		void loaddb(const nguid& aid)
		{
			np_actordb_load<DBTYPE, TDBTAB> ldata;
			ldata.m_id = aid;
			nets::sendbyserver(dbnodeid(), ldata, dbguid(), m_actor->id_guid());
			log_error()->print("ndbclient loaddb [{}] [{}]", m_name, aid);
		}

		nguid										m_id = nguid::make();
		std::map<nguid, data_modified<TDBTAB>>		m_data;
		data_modified<TDBTAB>*						m_dbdata = nullptr;
		bool										m_load = false;
		actor_manage_dbclient*						m_manage_dbclient = nullptr;
		actor_base*									m_actor = nullptr;
		std::vector<int64_t>						m_dellist;									// ɾ���б�
		std::string									m_name = tools::type_name<type_ndbclient>();// ��Ҫ������Ҫ֪��TACTOR������
	public:
		ndbclient():
			ndbclient_base(DBTYPE)
		{
		}

		void create(const nguid& aid) final
		{
			m_dbdata = &m_data[aid];
			m_dbdata->get()->set_mid(aid);
			m_id = aid;
		}

		void set_id(const nguid& aid)
		{ 
			m_id = aid;
		}

		// # ���ó��д����ݵ�actor
		void set_actor(actor_base* aactor)
		{
			m_manage_dbclient = aactor->get_actor_manage_dbclient();
			m_actor = aactor;
		}

		// # ��ȡ����
		std::map<nguid, data_modified<TDBTAB>>& get_data()
		{ 
			return m_data; 
		}

		// # ��ȡnguid����
		data_modified<TDBTAB>* get_data(const nguid& aid)
		{
			if (aid == m_id && m_id != nguid::make())
			{
				return m_dbdata;
			}
			return tools::findmap(m_data, aid);
		}

		// # ��ȡ����
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

			m_tab = ttab_dbload::instance().get_tabdb<TDBTAB>();
			if (m_tab == nullptr)
			{
				tools::no_core_dump();
				return;
			}

			static std::atomic<bool> lregister = true;
			if (lregister.exchange(false))
			{
				nrfun<TACTOR>::instance()
					.template rfun<actor_base, np_actordb_load_response<DBTYPE, TDBTAB>>(
						&actor_base::template handle<DBTYPE, TDBTAB, TACTOR>, false
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
				for (const auto& item : ldata)
				{
					lpdata = tools::findmap(m_data, nguid(item.first));
					if (lpdata == nullptr)
					{
						continue;
					}
					*lpdata->get(true, false) = item.second;
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
					// # ��ձ�־λ 
					lpdata->clear_modified();
				}			
			}
			else
			{
				std::set<i64_actorid>& lmodified = nmodified<TDBTAB>::which_modified();
				for (i64_actorid lactorid : lmodified)
				{
					lpdata = tools::findmap(m_data, nguid(lactorid));
					pro.add(lactorid, *lpdata->getconst(false));
				}
				// # ��ձ�־λ 
				lmodified.clear();
			}
			if (!pro.empty())
			{
				log_error()->print("ndbclient<{}> save {}", m_name, aid);
				// # �����л� ����actor_clientȷ��λ��
				i64_actorid lactorid = dbguid();
				std::shared_ptr<pack> lpack = actor_base::net_pack(pro, lactorid, m_actor->guid());
				if (lpack == nullptr)
				{
					log_error()->print("ndbclient<{}> actor_base::net_pack fail", m_name);
					return;
				}
				// # �첽����pack
				m_actor->send_actor_pack(lactorid, lpack);
			}
		}

		void del(const nguid& aid)
		{
			m_dellist.push_back((int64_t)aid);
			m_data.erase((int64_t)aid);
			if (aid == m_id)
			{
				m_dbdata = nullptr;
			}
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
				// # �����л� ����actor_clientȷ��λ��
				i64_actorid lactorid = dbguid();
				std::shared_ptr<pack> lpack = actor_base::net_pack(pro, lactorid, m_actor->guid());
				if (lpack == nullptr)
				{
					log_error()->print("ndbclient<{}> actor_base::net_pack fail", m_name);
					return;
				}
				// # �첽����pack
				m_actor->send_actor_pack(lactorid, lpack);
			}
		}

		void nscript_push_data() final
		{
			if (!m_actor->nscript_using())
			{
				return;
			}

			actor_base::nscript_data_db<TDBTAB> ltemp;
			for (std::pair<const nguid, data_modified<TDBTAB>>& item : m_data)
			{
				ltemp.data.insert(std::make_pair((int64_t)item.first, item.second.get(false, false)));
			}
			m_actor->nscript_data_push("db", ltemp, true);
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

		// # �������
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
			{//����ʧ��  ���ݿ���û������
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
		tmap_dbclient					m_dbclientmap;			//�Ѿ��������
		std::function<void(bool)>		m_fun;					//bool db���ݿ��Ƿ��и�����
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

			// 1���������޸�Ϊ[�ü��޸�]
			for (std::pair<const pbdb::ENUM_DB, ndbclient_base*>& lpair : m_dbclientmap)
			{
				lpair.second->clear_modified();
			}

			// 2������ѹ���ű���
			nscript_push_data();

			// 3����һЩ��ʼ��֮��Ĺ���,������Ҫ�Ļ����䷢�͸��ͻ���
			// 3.1 c++�ڲ�����
			m_fun(adbishave);
			// 3.2 ֪ͨ�ű�
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

}//namespace ngl

#define data_modified_return_get(NAME, DATA, ...)			\
	auto NAME = (DATA).get();								\
	if(NAME == nullptr)										\
	{														\
		return __VA_ARGS__;									\
	}

#define data_modified_return_getconst(NAME, DATA, ...)		\
	auto NAME = (DATA).getconst();							\
	if(NAME == nullptr)										\
	{														\
		return __VA_ARGS__;									\
	}

#define data_modified_break_get(NAME, DATA)					\
	auto NAME = (DATA).get();								\
	if(NAME == nullptr)										\
	{														\
		break;												\
	}

#define data_modified_break_getconst(NAME, DATA)			\
	auto NAME = (DATA).getconst();							\
	if(NAME == nullptr)										\
	{														\
		break;												\
	}

#define data_modified_continue_get(NAME, DATA)				\
	auto NAME = (DATA).get();								\
	if(NAME == nullptr)										\
	{														\
		continue;											\
	}

#define data_modified_continue_getconst(NAME, DATA)			\
	auto NAME = (DATA).getconst();							\
	if(NAME == nullptr)										\
	{														\
		continue;											\
	}

#define data_modified_dump_get(NAME, DATA)					\
	auto NAME = (DATA).get();								\
	if(NAME == nullptr)										\
	{														\
		ngl::tools::tools::no_core_dump();					\
	}

#define data_modified_dump_getconst(NAME, DATA)				\
	auto NAME = (DATA).getconst();							\
	if(NAME == nullptr)										\
	{														\
		ngl::tools::tools::no_core_dump();					\
	}