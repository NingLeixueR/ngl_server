#pragma once

#include "ndbclient.h"
#include "csvtable.h"
#include "nlog.h"

namespace ngl
{
	class ndb_component
	{
		ndb_component() = delete;
		ndb_component(const ndb_component&) = delete;
		ndb_component& operator=(const ndb_component&) = delete;

	protected:
		actor_base*				m_actor;
		i64_actorid				m_id;
		ndbclient_base*			m_dbclient;
		pbdb::ENUM_DB			m_type;

		ndb_component(pbdb::ENUM_DB aenum);
		void				set_dbclient(ndbclient_base* adbclient);
	public:
		void				set(actor_base* aactor);
		i64_actorid			id()const;
		pbdb::ENUM_DB		type()const;
		actor_base*			actorbase();
		virtual void		set_id();
		void				init();
		void				create();
		ndbclient_base*		dbclientbase();
		// # 当数据全部加载后调用
		virtual void		init_data();
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	class ndb_modular : public ndb_component
	{
		ndb_modular(const ndb_modular&) = delete;
		ndb_modular& operator=(const ndb_modular&) = delete;

	protected:
		ndbclient<ENUM, TDATA, TACTOR> m_data;

		inline ndb_modular():
			ndb_component(ENUM)
		{
			set_dbclient(&m_data);
		}

		virtual ~ndb_modular()
		{
		}

	public:
		inline ndbclient<ENUM, TDATA, TACTOR>* dbclient()
		{ 
			return &m_data;
		}

		inline void foreach(const std::function<void(data_modified<TDATA>&)>& afun)
		{
			for (std::pair<const nguid, data_modified<TDATA>>& lpair : data())
			{
				afun(lpair.second);
			}
		}

		inline std::map<nguid, data_modified<TDATA>>& data()
		{
			return m_data.get_data();
		}

		inline data_modified<TDATA>* get()
		{
			return get(m_id);
		}

		// # 查找指定数据
		inline data_modified<TDATA>* find(nguid aid)
		{
			auto itor = data().find(aid);
			if (itor == data().end())
				return nullptr;
			return &itor->second;
		}

		// # 与find类似(只是没有就添加)
		inline data_modified<TDATA>* get(nguid aid)
		{
			data_modified<TDATA>* ret = find(aid);
			if (ret != nullptr)
				return ret;
			if (m_id != -1 && m_id != aid)
				return nullptr;
			data_modified<TDATA>& ldata = data()[aid];
			ldata.get().set_m_id(aid);
			return &ldata;
		}
		
		// # 删除指定数据
		inline void erase(nguid aid)
		{
			m_data.del(aid);
		}

		inline data_modified<TDATA>* db()
		{ 
			return m_data.get_dbdata();
		}

		inline TACTOR* actor()
		{ 
			return (TACTOR*)m_actor; 
		}

#pragma region log
		inline std::shared_ptr<np_actor_logitem> log_debug(
			const std::source_location& asource = std::source_location::current()
		)
		{
			return actor()->log_debug(asource);
		}

		inline std::shared_ptr<np_actor_logitem> log_debug_net(
			const std::source_location& asource = std::source_location::current()
		)
		{
			return actor()->log_debug_net(asource);
		}

		inline std::shared_ptr<np_actor_logitem> log_info(
			const std::source_location& asource = std::source_location::current()
		)
		{
			return actor()->log_info(asource);
		}

		inline std::shared_ptr<np_actor_logitem> log_info_net(
			const std::source_location& asource = std::source_location::current()
		)
		{
			return actor()->log_info_net(asource);
		}

		inline std::shared_ptr<np_actor_logitem> log_warn(
			const std::source_location& asource = std::source_location::current()
		)
		{
			return actor()->log_warn(asource);
		}

		inline std::shared_ptr<np_actor_logitem> log_warn_net(
			const std::source_location& asource = std::source_location::current()
		)
		{
			return actor()->log_warn_net(asource);
		}

		inline std::shared_ptr<np_actor_logitem> log_error(
			const std::source_location& asource = std::source_location::current()
		)
		{
			return actor()->log_error(asource);
		}

		inline std::shared_ptr<np_actor_logitem> log_error_net(
			const std::source_location& asource = std::source_location::current()
		)
		{
			return actor()->log_error_net(asource);
		}
#pragma endregion

		inline void create()
		{
			m_id = m_actor->id_guid();
			m_data.set_id(m_id);
			m_data.set_actor(m_actor);
			m_data.add(m_id, TDATA());
		}

		inline void clear()
		{
			m_data.set(m_id, TDATA());
		}

		// 当数据全部加载后调用
		virtual void init_data() final;

		virtual void initdata() = 0;

		inline ndbclient<ENUM, TDATA, TACTOR>* get_actor_dbclient()
		{
			return (ndbclient<ENUM, TDATA, TACTOR>*)m_dbclient;
		}

		inline data_modified<TDATA>* add(i64_actorid aid, const TDATA& adbtab)
		{
			return m_data.add(aid, adbtab);
		}

		inline void remove(int64_t aid)
		{
			get_actor_dbclient()->del(aid);
		}
	};
}//namespace ngl