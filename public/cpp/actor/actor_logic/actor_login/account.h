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
// File overview: Account data structures for login session tracking and credential validation.

#pragma once

#include "actor/generated/auto/nactor_auto.h"

namespace ngl
{
	/**
	 * Account management module for player login system.
	 *
	 * This class manages player accounts, including account creation, lookup by name,
	 * and area-based account organization. It extends the database modular system
	 * to provide persistent storage for account data.
	 */
	class account :
		public tdb_account::db_modular
	{
		account(const account&) = delete;
		account& operator=(const account&) = delete;

		/**
		 * Helper structure to organize accounts by name within an area.
		 * Provides fast lookup of account data by account name string.
		 */
		struct accountbyname
		{
			std::map<std::string, data_modified<pbdb::db_account>*> m_data;
		};

		// Map of area ID to account-by-name lookup structure
		std::map<i16_area, accountbyname> m_areaofaccount;

		// Maximum account ID assigned so far (used for generating new account IDs)
		int32_t m_max_accountid;
	public:
		account();

		/**
		 * Override to set up the actor ID relationship for this DB component.
		 * Called during initialization to bind the component to its owning actor.
		 */
		void related_actorid() final;

		/**
		 * Initialize account data structures after database load.
		 * Builds the area-based and name-based lookup indices.
		 */
		virtual void initdata();

		/**
		 * Get or set the maximum account ID counter.
		 * @return Reference to the maximum account ID
		 */
		int32_t& max_accountid();

		/**
		 * Get the account-by-name lookup map for a specific area.
		 * @param area The area ID to query
		 * @return Reference to the map of account names to account data pointers
		 */
		std::map<std::string, data_modified<pbdb::db_account>*>& accountbyarea(int area);
	};
}//namespace ngl

mk_formatter(pbdb::db_account)