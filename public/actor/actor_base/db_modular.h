#pragma once

#include "actor_db_client.h"
#include "csvtable.h"
#include "nlog.h"

namespace ngl
{
	class db_component
	{
	protected:
		actor_base* m_actor;
		i64_actorid m_id;
		actor_dbclient_base* m_dbclient;
		pbdb::ENUM_DB m_type;

		db_component(pbdb::ENUM_DB aenum);
		void		set_dbclient(actor_dbclient_base* adbclient);
	public:
		void		set(actor_base* aactor);
		i64_actorid id();
		pbdb::ENUM_DB		type();
		actor_base* actorbase();
		virtual void set_id();
		void		init();
		void		create();
		actor_dbclient_base* dbclientbase();
		// 当数据全部加载后调用
		virtual void init_data();
	};

	template <
		EPROTOCOL_TYPE PROTYPE, 
		pbdb::ENUM_DB ENUM,
		typename TDATA, 
		typename TACTOR
	>
	class db_modular : public db_component
	{
	protected:
		actor_dbclient<PROTYPE, ENUM, TDATA, TACTOR> m_data;
		db_modular():
			db_component(ENUM)
		{
			set_dbclient(&m_data);
		}
		~db_modular()
		{
		}
	public:
		actor_dbclient<PROTYPE, ENUM, TDATA, TACTOR>* dbclient()
		{ 
			return &m_data;
		}

		void foreach(const std::function<void(data_modified<TDATA>&)>& afun)
		{
			for (auto& [_, value] : m_data.m_data)
			{
				afun(value);
			}
		}

		std::map<actor_guid, data_modified<TDATA>>& data()
		{
			return m_data.m_data;
		}

		data_modified<TDATA>* get()
		{
			return get(m_id);
		}

		// 没有就添加
		data_modified<TDATA>* get(actor_guid aid)
		{
			data_modified<TDATA>* ret = find(aid);
			if (ret != nullptr)
				return ret;
			data_modified<TDATA>& ldata = m_data.m_data[aid];
			ldata.get().set_m_id(aid);
			return &ldata;
		}

		data_modified<TDATA>* find(actor_guid aid)
		{
			auto itor = m_data.m_data.find(aid);
			if (itor == m_data.m_data.end())
				return nullptr;
			return &itor->second;
		}

		void erase(actor_guid aid)
		{
			m_data.del(aid);
		}

		data_modified<TDATA>* db()
		{ 
			return m_data.m_dbdata; 
		}

		TACTOR* actor()							
		{ 
			return (TACTOR*)m_actor; 
		}

		void create()
		{
			m_id = m_actor->id_guid();
			m_data.set_id(m_id);
			m_data.set_actor(m_actor);
			m_data.add(m_id, TDATA());
		}

		void clear()
		{
			m_data.set(m_id, TDATA());
		}

		// 当数据全部加载后调用
		virtual void init_data() final
		{
			initdata();
		}

		virtual void initdata() = 0;

		actor_dbclient<PROTYPE, ENUM, TDATA, TACTOR>* get_actor_dbclient()
		{
			return (actor_dbclient<PROTYPE, ENUM, TDATA, TACTOR>*)m_dbclient;
		}

		data_modified<TDATA>* add(i64_actorid aid, const TDATA& adbtab)
		{
			return m_data.add(aid, adbtab);
		}

		void remove(int64_t aid)
		{
			get_actor_dbclient()->del(aid);
		}
	};

}