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
// File overview: Declares interfaces for sql.

#pragma once

#include "tools/serialize/nserialize.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "tools/type.h"

#include <set>

namespace ngl
{
	template <typename T>
	struct db_data
	{
		db_data() = delete;
		db_data(const db_data&) = delete;
		db_data& operator=(const db_data&) = delete;
	private:
		// Loaded rows currently kept in memory.
		static std::map<i64_actorid, T>		m_data;
		// Known ids are tracked separately so callers can distinguish "not
		// loaded yet" from "definitely does not exist".
		static std::set<int64_t>			m_idindex;
	public:
		// Read-only access to the known-id index.
		static const std::set<int64_t>& const_id_index()
		{
			return m_idindex;
		}

		// Mutable access used during DB bootstrap and id preloading.
		static std::set<int64_t>& id_index()
		{
			return m_idindex;
		}

		// Mark one id as known to exist.
		static void set_index(int64_t aid)
		{
			m_idindex.insert(aid);
		}

		// Remove one id from the known-id index.
		static void erase_index(int64_t aid)
		{
			m_idindex.erase(aid);
		}

		// Cache or replace one loaded row.
		static bool set(i64_actorid aid, const T& adata)
		{
			m_data[aid] = adata;
			set_index(aid);
			return true;		
		}

		// Remove one row from both the loaded cache and the known-id index.
		static void remove(i64_actorid aid)
		{
			m_data.erase(aid);
			erase_index(aid);
		}

		// Remove a batch of rows addressed as a set.
		static void remove(const std::set<i64_actorid>& aid)
		{
			for (i64_actorid id : aid)
			{
				m_data.erase(id);
				erase_index(id);
			}				
		}

		// Remove a batch of rows addressed as a vector.
		static void remove(const std::vector<i64_actorid>& aid)
		{
			for (i64_actorid id : aid)
			{
				m_data.erase(id);
				erase_index(id);
			}
		}

		// Distinguishes cache miss, known-but-not-loaded, and fully loaded rows.
		enum edbdata
		{
			edbdata_notload,	// Id exists, but the full row is not loaded yet.
			edbdata_load,		// Full row is loaded in memory.
			edbdata_notdata,	// Id is unknown and treated as missing.
		};
		static edbdata data_stat(i64_actorid aid)
		{
			auto itor = m_idindex.find(aid);
			if (itor == m_idindex.end())
			{
				return edbdata_notdata;
			}
			if (m_data.find(aid) == m_data.end())
			{
				return edbdata_notload;
			}
			return edbdata_load;
		}

		// Find one loaded row without triggering a DB fetch.
		static T* find(i64_actorid aid)
		{
			auto itor = m_data.find(aid);
			if (itor == m_data.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		static bool add(i64_actorid aid, T& adata)
		{
			// `mid` is normalized to the map key before the object is cached.
			auto itor = m_data.find(aid);
			if (itor != m_data.end())
			{
				return false;
			}
			T& ldata = m_data[aid];
			ldata.set_mid(aid);
			ldata = adata;
			set_index(aid);
			return &ldata;
		}

		static void foreach(const std::function<void(T&)>& afun)
		{
			for (std::pair<const i64_actorid, T>& item : m_data)
			{
				afun(item.second);
			}
		}

		static void foreach_index(const std::function<void(int, T&)>& afun)
		{
			int lindex = 0;
			for (std::pair<const i64_actorid, T>& lpair : m_data)
			{
				afun(++lindex, lpair.second);
			}				
		}

		static void foreach_index(int32_t aindexbeg, int32_t aindexend, const std::function<void(int32_t, T&)>& afun)
		{
			int lindex = 0;
			for (std::pair<const i64_actorid, T>& lpair : m_data)
			{
				++lindex;
				if (lindex >= aindexbeg && lindex < aindexend)
				{
					afun(lindex, lpair.second);
				}
				if (lindex > aindexend)
				{
					return;
				}
			}
		}
	};

	template <typename T>
	std::map<i64_actorid, T> db_data<T>::m_data;

	template <typename T>
	std::set<int64_t> db_data<T>::m_idindex;
}// namespace ngl
