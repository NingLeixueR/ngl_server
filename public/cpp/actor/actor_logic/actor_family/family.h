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
// File overview: Declares interfaces for actor family.

#pragma once

#include "actor/tab/ttab_specialid.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tools/tools_localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class familyer : 
		public tdb_familyer::db_modular
	{
		familyer(const familyer&) = delete;
		familyer& operator=(const familyer&) = delete;

	public:
		familyer() = default;

		void related_actorid()final
		{
			set_actorid(nguid::make());
		}

		void initdata()final
		{
			log_error()->print("{}", data());
		}

		// Whethercancreateguild
		bool check_createfamily(i64_actorid aroleid)
		{
			data_modified<pbdb::db_familyer>& lpdbfamilyer = get(aroleid);
			auto lnow = (int32_t)tools::localtime::gettime();
			const pbdb::db_familyer* lpdbfamilyerconst = lpdbfamilyer.getconst();
			if (lpdbfamilyerconst == nullptr)
			{
				return false;
			}
			if (lnow - lpdbfamilyerconst->mlastleaveutc() < ttab_specialid::instance().m_familjoininterval)
			{
				return false;
			}
			return true;
		}
	};

	class family : 
		public tdb_family::db_modular
	{
		family(const family&) = delete;
		family& operator=(const family&) = delete;

		int32_t									m_maxid = 0;
		std::map<int64_t, int64_t>				m_rolefamily;	// key:roleid value:familyid
		std::map<int64_t, std::set<int64_t>>	m_applylist;	// key:roleid value:std::set<familyid>
		std::set<std::string>					m_familyname;	// Used to guildnamewhether
	public:
		family():
			m_maxid(0)
		{
		}

		void related_actorid()final
		{
			set_actorid(nguid::make());
		}

		void initdata()final
		{
			log_error()->print("{}", data());
			//td::map<nguid, data_modified<TDATA>>
			for (auto& lpair : data())
			{
				const nguid& lguid = lpair.first;
				auto& ldata = lpair.second;
				const pbdb::db_family* lpdbfamilyconst = ldata.getconst();
				if (lpdbfamilyconst == nullptr)
				{
					continue;
				}
				m_maxid = std::max(m_maxid, static_cast<int32_t>(lguid.id()));
				
				for (i64_actorid lroleid : lpdbfamilyconst->mmember())
				{
					m_rolefamily.insert_or_assign(lroleid, lguid);
				}

				m_familyname.insert(lpdbfamilyconst->mname());

				for (i64_actorid lroleid : lpdbfamilyconst->mapplylist())
				{
					m_applylist[lroleid].insert(lguid);
				}
			}
		}

		// # Createguild
		int32_t create_family(i64_actorid aroleid, const std::string& aname);

		// # Applyjoinguild
		int32_t join_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # Fromguild[afamilyid]inapplylistinremove[aroleid]
		int32_t erase_applylist(i64_actorid aroleid, i64_actorid afamilyid);

		// # Canceljoinguild
		int32_t cancel_join_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # Guild accept/rejectplayerjoinguild
		int32_t ratify_join_family(i64_actorid aroleid, i64_actorid ajoinroleid, bool aratify);

		// # Guild transferguild
		int32_t cede_family(i64_actorid aroleid, i64_actorid acederoleid);

		// # Exitguild
		int32_t leave_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # Guildname
		int32_t change_familyname(i64_actorid aroleid, i64_actorid afamilyid, const std::string& afamilyname);

		// # Guildsign in
		int32_t sign_family(i64_actorid aroleid, i64_actorid afamilyid);

		bool get_familylist(i64_actorid afamilyid, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& apro);

		void sync_familylist(i64_actorid aroleid, i64_actorid afamilyid);

		void sync_family(i64_actorid aroleid);

		// # Getguildall
		bool get_familyers(i64_actorid aroleid, std::vector<i64_actorid>& afamilyers);
	};
}// namespace ngl

mk_formatter(pbdb::db_familyer)
mk_formatter(pbdb::db_family)
