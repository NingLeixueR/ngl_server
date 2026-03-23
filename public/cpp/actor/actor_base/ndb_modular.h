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

#include "actor/actor_base/ndbclient.h"
#include "actor/tab/csvtable.h"
#include "tools/log/nlog.h"

namespace ngl
{
	class ndb_component
	{
		ndb_component() = delete;
		ndb_component(const ndb_component&) = delete;
		ndb_component& operator=(const ndb_component&) = delete;

	private:
		actor_base*				m_actor		= nullptr;							// Actor that owns this DB component.
		ndbclient_base*			m_dbclient  = nullptr;							// Storage backend bound to this component.
		i64_actorid				m_id		= 0;								// Actor id used as the primary DB key scope.
		pbdb::ENUM_DB			m_type		= pbdb::ENUM_DB::ENUM_DB_FAIL;		// Logical DB table/type served by this component.

	protected:
		explicit ndb_component(pbdb::ENUM_DB aenum);

		// Bind the concrete dbclient used by this component.
		void set_dbclient(ndbclient_base* andbclient);

		// Access the bound dbclient without changing ownership.
		ndbclient_base* get_dbclient();
	public:
		// Attach this component to its host actor and register it for DB lifecycle callbacks.
		void				set(actor_base* aactor);

		// Return the host actor.
		actor_base*			get_actor();

		// Return the actor id currently associated with this component.
		i64_actorid			get_actorid()const;

		// Return the DB enum served by this component.
		pbdb::ENUM_DB		type()const;

		// Convenience alias for get_actor().
		actor_base*			actorbase();

		// Override the actor id used as the DB lookup key.
		void				set_actorid(i64_actorid aactorid);

		// Default actor-id binding: use the owning actor's guid.
		virtual void		related_actorid();

		// Finalize actor/dbclient binding and register the dbclient with actor_base.
		void				init();

		// Create initial DB records for the current actor id or an explicit id.
		void				create();
		void				create(const nguid& aid);

		// Return the raw dbclient pointer.
		ndbclient_base*		dbclientbase();

		// Hook called after all DB tables have finished loading.
		virtual void		init_data();
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	class ndb_modular : 
		public ndb_component
	{
		ndb_modular(const ndb_modular&) = delete;
		ndb_modular& operator=(const ndb_modular&) = delete;

	private:
		ndbclient<ENUM, TDATA, TACTOR> m_data;
	protected:
		inline ndb_modular():
			ndb_component(ENUM)
		{
			set_dbclient(&m_data);
		}

		virtual ~ndb_modular() = default;
	public:
		inline ndbclient<ENUM, TDATA, TACTOR>* dbclient()
		{ 
			return &m_data;
		}

		// Iterate over every loaded row owned by this module.
		inline void foreach(const std::function<void(const data_modified<TDATA>&)>& afun)
		{
			for (auto& [_guid, _datamodified] : data())
			{
				afun(_datamodified);
			}
		}

		// Find the first row that matches a custom predicate.
		inline data_modified<TDATA>* find(const std::function<bool(const data_modified<TDATA>&)>& afun)
		{
			for(auto& [_guid, _datamodified] : data())
			{
				if (afun(_datamodified))
				{
					return &_datamodified;
				}
			}
			return nullptr;
		}

		// Expose the full loaded dataset as a read-only map.
		inline std::map<nguid, data_modified<TDATA>>& data()
		{
			return m_data.get_data();
		}

		inline const std::map<nguid, data_modified<TDATA>>& data() const
		{
			return m_data.get_data();
		}

		inline std::map<nguid, data_modified<TDATA>>& foreach_data()
		{
			return m_data.get_foreach_data();
		}

		// Return the row bound to the owning actor id.
		inline data_modified<TDATA>& get()
		{
			return get(get_actorid());
		}

		// Find a specific row by actor guid.
		inline data_modified<TDATA>* find(nguid aid)
		{
			auto itor = data().find(aid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		// Get or lazily create one row by actor guid.
		inline data_modified<TDATA>& get(nguid aid)
		{
			if (aid == nguid::make())
			{
				tools::no_core_dump();
			}
			if (get_actorid() != nguid::make() && get_actorid() != aid)
			{
				tools::no_core_dump();
			}
			return *m_data.get_data(aid);
		}

		// Mark one row for deletion.
		inline void erase(nguid aid)
		{
			get_actor_dbclient()->del(aid, true);
		}

		// Return the primary row when this module is bound to one actor id.
		inline data_modified<TDATA>* db()
		{ 
			return m_data.get_dbdata();
		}

		// Typed access to the owning actor.
		inline TACTOR* nactor()
		{ 
			return (TACTOR*)get_actor();
		}

		// Delete every row currently tracked by this module.
		inline void clear()
		{
			get_actor_dbclient()->clear();
		}

		// Adapter hook from ndb_component into the derived initdata() implementation.
		void init_data() final;

		virtual void initdata() = 0;

		inline ndbclient<ENUM, TDATA, TACTOR>* get_actor_dbclient()
		{
			return (ndbclient<ENUM, TDATA, TACTOR>*)get_dbclient();
		}

		inline data_modified<TDATA>& add(i64_actorid aid, const TDATA& adbtab)
		{
			if (get_actorid() != nguid::make() && get_actorid() != aid)
			{
				tools::no_core_dump();
			}
			data_modified<TDATA>& ldata = get(aid);
			*ldata.get() = adbtab;
			return ldata;
		}

		inline void remove(int64_t aid)
		{
			get_actor_dbclient()->del(aid);
		}
	};
}//namespace ngl
