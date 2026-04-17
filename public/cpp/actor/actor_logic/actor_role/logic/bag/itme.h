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
// File overview: Item data structures, type classification, and stack operations.

#pragma once

#include "actor/tab/ttab_item.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/generated/pb/db.pb.h"

namespace ngl
{
	class actor_role;

	/**
	 * Factory pattern base class for creating different types of items.
	 *
	 * This class implements a registry pattern where each item type registers
	 * its own creation logic. When an item needs to be created, the factory
	 * looks up the appropriate creator based on the item's type and delegates
	 * the creation to the registered handler.
	 *
	 * Usage:
	 * - Derive from this class and implement the mode() method for each item type
	 * - Register the creator in the constructor by passing the item type
	 * - Call create() to instantiate items of any registered type
	 */
	class item_create
	{
		// Registry mapping item types to their creation handlers
		static std::map<EItemType, item_create*> m_data;
	protected:
		/**
		 * Constructor that registers this creator for a specific item type.
		 * @param atype The item type this creator handles
		 */
		explicit item_create(EItemType atype)
		{
			m_data[atype] = this;
		}

		/**
		 * Pure virtual method to implement item creation logic for a specific type.
		 *
		 * @param arole The role actor creating the item
		 * @param atid The item template ID from configuration
		 * @param acount The number of items to create
		 * @param avec Output vector to store the created item data
		 * @return True if creation succeeded, false otherwise
		 */
		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec) = 0;
	public:
		/**
		 * Static factory method to create items of any registered type.
		 *
		 * @param arole The role actor creating the item
		 * @param atid The item template ID from configuration
		 * @param acount The number of items to create
		 * @param avec Output vector to store the created item data
		 * @return True if creation succeeded, false if item type not found or creation failed
		 */
		static bool create(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			// Look up item configuration
			auto tab = ttab_item::instance().tab(atid);
			if (tab == nullptr)
			{
				return false;
			}
			// Find the registered creator for this item type
			auto itor = m_data.find((EItemType)tab->m_type);
			if (itor == m_data.end())
			{
				return false;
			}
			// Delegate to the type-specific creator
			return itor->second->mode(arole, atid, acount, avec);
		}
	};
}// namespace ngl