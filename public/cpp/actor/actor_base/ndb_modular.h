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
		actor_base*				m_actor		= nullptr;							// Hostactor
		ndbclient_base*			m_dbclient  = nullptr;							// Ndbclientbase class
		i64_actorid				m_id		= 0;								// Hostactor id
		pbdb::ENUM_DB			m_type		= pbdb::ENUM_DB::ENUM_DB_FAIL;		// Datatype

	protected:
		explicit ndb_component(pbdb::ENUM_DB aenum);

		// # Setndbclientbase class
		void set_dbclient(ndbclient_base* andbclient);

		// # Get the ndbclient base class
		inline ndbclient_base* get_dbclient();
	public:
		// # Sethostactor
		void				set(actor_base* aactor);

		// # Getactor
		actor_base*			get_actor();

		// # Get the host actor id
		i64_actorid			get_actorid()const;

		// # Getdatatype
		pbdb::ENUM_DB		type()const;

		// # Get the host actor
		actor_base*			actorbase();

		// # Related_actorid,used to actor id
		void				set_actorid(i64_actorid aactorid);

		// # Setm_id m_actor
		virtual void		related_actorid();

		// # Setm_actor m_dbclient
		void				init();

		// # Createdata
		void				create();
		void				create(const nguid& aid);

		// # Getm_dbclient
		ndbclient_base*		dbclientbase();

		// # Dataallloadafter
		virtual void		init_data();
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	class ndb_modular : 
		public ndb_component
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

		virtual ~ndb_modular() = default;
	public:
		inline ndbclient<ENUM, TDATA, TACTOR>* dbclient()
		{ 
			return &m_data;
		}

		// # Data
		inline void foreach(const std::function<void(const data_modified<TDATA>&)>& afun)
		{
			for (auto& [_guid, _datamodified] : data())
			{
				afun(_datamodified);
			}
		}

		// # Find specified data
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

		// # Get all data
		inline const std::map<nguid, data_modified<TDATA>>& data()
		{
			return m_data.get_data();
		}

		inline std::map<nguid, data_modified<TDATA>>& foreach_data()
		{
			return m_data.get_foreach_data();
		}

		// # Getm_idcorresponding data
		inline data_modified<TDATA>& get()
		{
			return get(get_actorid());
		}

		// # Finds pecified data
		inline data_modified<TDATA>* find(nguid aid)
		{
			auto itor = data().find(aid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return const_cast<data_modified<TDATA>*>(&itor->second);
		}

		// # And find similar( add)
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

		// # Delete specified data
		inline void erase(nguid aid)
		{
			get_actor_dbclient()->del(aid, true);
		}

		// # Return data
		inline data_modified<TDATA>* db()
		{ 
			return m_data.get_dbdata();
		}

		// # Get the host actor
		inline TACTOR* nactor()
		{ 
			return (TACTOR*)get_actor();
		}

		// # Clear data
		inline void clear()
		{
			get_actor_dbclient()->clear();
		}

		// # Data all loadafter
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