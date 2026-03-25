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
// File overview: Implements logic for nsp.

#include "actor/actor_base/nsp/nsp.h"
#include "tools/pb_field.h"

namespace ngl
{
	void nsp_regload::init(i64_actorid aactorid)
	{
		m_nspserver.clear();
		m_register.clear();
		m_loadfinish.clear();

		// One NSP server exists per area for a given actor type.
		std::set<i16_area> lareaset;
		ttab_servers::instance().get_arealist_nonrepet(nconfig.tid(), lareaset);
		auto ltype = (ENUM_ACTOR)nguid::type(aactorid);
		for (i16_area larea : lareaset)
		{
			m_nspserver.emplace(larea, nguid::make(ltype, larea, nguid::none_actordataid()));
			m_register.emplace(larea, false);
			m_loadfinish.emplace(larea, false);
		}
	}

	void nsp_regload::set_register(i16_area aarea)
	{
		bool* lpreg = tools::findmap(m_register, aarea);
		if (lpreg == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		*lpreg = true;
	}

	bool nsp_regload::is_register(i16_area aarea)const
	{
		auto lpregister = tools::findmap(m_register, aarea);
		if (lpregister == nullptr)
		{
			return false;
		}
		return *lpregister;
	}

	bool nsp_regload::is_register()const
	{
		for (const auto& lpair : m_register)
		{
			if (!lpair.second)
			{
				return false;
			}
		}
		return true;
	}

	void nsp_regload::set_loadfinish(i16_area aarea)
	{
		bool* lpfinish = tools::findmap(m_loadfinish, aarea);
		if (lpfinish == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		*lpfinish = true;
	}

	bool nsp_regload::is_loadfinish(i16_area aarea)const
	{
		auto lpbool = tools::findmap(m_loadfinish, aarea);
		if (lpbool == nullptr)
		{
			return false;
		}
		return *lpbool;
	}

	i64_actorid nsp_regload::nspserid(i16_area aarea)const
	{
		auto lpserid = tools::findmap(m_nspserver, aarea);
		if (lpserid == nullptr)
		{
			tools::no_core_dump();
			return nguid::make();
		}
		return *lpserid;
	}

	bool nsp_regload::is_loadfinish()const
	{
		for (const auto& lpair : m_loadfinish)
		{
			if (!lpair.second)
			{
				return false;
			}
		}
		return true;
	}

	void nsp_regload::foreach_nspser(const std::function<void(i16_area, i64_actorid)>& afun)const
	{
		for (const auto& lpair : m_nspserver)
		{
			afun(lpair.first, lpair.second);
		}
	}

	void operator_field::init(bool anspserver)
	{
		m_node_fieldnumbers.clear();
		m_nspserver = anspserver;
	}

	void operator_field::set_field(i16_actortype atype, const std::map<i32_fieldnumber, epb_field>& anode_fieldnumbers)
	{
		auto [ltype_it, lnew_type] = m_node_fieldnumbers.try_emplace(atype, anode_fieldnumbers);
		if (lnew_type)
		{
			return;
		}

		// Merge repeated announcements from multiple peers while keeping role consistency.
		std::map<i32_fieldnumber, epb_field>& lmap = ltype_it->second;
		for (const auto& [lfieldnum, lfieldtype] : anode_fieldnumbers)
		{
			auto [lfield_it, lnew_field] = lmap.try_emplace(lfieldnum, lfieldtype);
			if (!lnew_field && lfield_it->second != lfieldtype)
			{
				tools::no_core_dump();
			}
		}
	}

	void operator_field::set_field(const std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& anode_fieldnumbers)
	{
		// Reuse the single-type merge path so conflict checks stay centralized.
		std::ranges::for_each(anode_fieldnumbers, [this](const auto& apair)
			{
				set_field(apair.first, apair.second);
			}
		);
	}

	void operator_field::nspser_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype)
	{
		epb_field* lpfield = tools::findmap(afieldmap, afieldnumber);
		if (lpfield != nullptr)
		{
			if (*lpfield == afieldtype)
			{
				// Server-side registration should not redundantly announce the same permission twice.
				tools::no_core_dump();
			}
		}
		afieldmap.insert_or_assign(afieldnumber, afieldtype);
	}

	void operator_field::nspcli_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype)
	{
		epb_field* lpfield = tools::findmap(afieldmap, afieldnumber);
		if (lpfield != nullptr)
		{
			if (*lpfield == epb_field_write)
			{
				// Write permission already implies read visibility on the client side.
				return;
			}
		}
		afieldmap.insert_or_assign(afieldnumber, afieldtype);
	}

	void operator_field::add_field(i16_actortype atype, i32_fieldnumber afieldnumber, epb_field afieldtype)
	{
		if (m_nspserver)
		{
			nspser_add_field(m_node_fieldnumbers[atype], afieldnumber, afieldtype);
		}
		else
		{
			nspcli_add_field(m_node_fieldnumbers[atype], afieldnumber, afieldtype);
		}
	}

	std::map<i32_fieldnumber, epb_field>* operator_field::get_field(i16_actortype atype)
	{
		auto it = m_node_fieldnumbers.find(atype);
		if (it != m_node_fieldnumbers.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& operator_field::field_numbers()
	{
		return m_node_fieldnumbers;
	}

	void care_data::init(bool aread)
	{
		m_core = nsp_care{};
		m_core.m_read = aread;
		// "all" means the explicit id sets are ignored.
		m_core.m_all = true;
	}

	void care_data::init(const std::set<i64_actorid>& aids)
	{
		m_core = nsp_care{};
		m_core.m_read = true;
		m_core.m_all = false;
		m_core.m_readids = aids;
	}

	void care_data::init(const std::set<i64_actorid>& areadids, const std::set<i64_actorid>& awriteids)
	{
		m_core = nsp_care{};
		m_core.m_read = false;
		m_core.m_all = false;
		m_core.m_readids = areadids;
		m_core.m_writeids = awriteids;
	}

	void care_data::init([[maybe_unused]] bool aread, const std::set<i64_actorid>& awriteids)
	{
		m_core = nsp_care{};
		m_core.m_read = false;
		m_core.m_all = true;
		m_core.m_writeids = awriteids;
	}

	void care_data::init(const nsp_care& acore)
	{
		m_core = acore;
	}

	bool care_data::is_care(i64_actorid adataid)const
	{
		if (m_core.m_read)
		{
			// Read-only subscriptions care only about the read set.
			return m_core.m_all || m_core.m_readids.contains(adataid);
		}
		else
		{
			// Read/write subscriptions care about both readable and writable rows.
			return m_core.m_all || m_core.m_writeids.contains(adataid) || m_core.m_readids.contains(adataid);
		}
	}

	bool care_data::is_read(i64_actorid adataid)const
	{
		return m_core.m_all || m_core.m_writeids.contains(adataid) || m_core.m_readids.contains(adataid);
	}

	bool care_data::is_write(i64_actorid adataid)const
	{
		if (m_core.m_read)
		{
			return false;
		}
		else
		{
			return m_core.m_all || m_core.m_writeids.contains(adataid);
		}
	}

	bool care_data::is_readall()const
	{
		return m_core.m_all;
	}

	bool care_data::is_writeall()const
	{
		if (m_core.m_read)
		{
			return false;
		}
		return m_core.m_all;
	}

	std::set<i64_actorid>& care_data::readids()
	{
		return m_core.m_readids;
	}

	std::set<i64_actorid>& care_data::writeids()
	{
		return m_core.m_writeids;
	}

	const nsp_care& care_data::get_core()const
	{
		return m_core;
	}
}//namespace ngl
