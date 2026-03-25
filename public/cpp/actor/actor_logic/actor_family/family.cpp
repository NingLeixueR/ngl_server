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
// File overview: Implements logic for actor family.


#include "actor/actor_logic/actor_family/actor_family.h"
#include "actor/actor_logic/actor_family/family.h"
#include "actor/tab/ttab_specialid.h"
#include "actor/tab/ttab_familylv.h"
#include "actor/tab/csvtable.h"

namespace ngl
{
	int32_t family::create_family(i64_actorid aroleid, const std::string& aname)
	{
		if (!m_rolefamily.contains(aroleid))
		{
			return 1;
		}
		if (nactor()->m_familyer.check_createfamily(aroleid) == false)
		{
			return 2;
		}
		if (m_familyname.contains(aname))
		{
			return 3;
		}

		pbdb::db_family ldbfamily;
		int32_t lnow = (int32_t)localtime::gettime();
		i64_actorid lfamilyid = nguid::make(ACTOR_FAMILY, tab_self_area, ++m_maxid);
		ldbfamily.set_mid(lfamilyid);
		ldbfamily.set_mcreateutc(lnow);
		ldbfamily.set_mname(aname);
		ldbfamily.set_mleader(aroleid);
		ldbfamily.set_mlv(0);
		ldbfamily.set_mexp(0);
		*ldbfamily.mutable_mmember()->Add() = aroleid;

		data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);
		MODIFIED_RETUAN(lpfamilyer, lpmodifiedfamilyer, 4);
		lpfamilyer->set_mid(aroleid);
		lpfamilyer->set_mlastsignutc(0);
		lpfamilyer->set_mjoinutc(lnow);
		lpfamilyer->set_mposition(pbdb::db_familyer_eposition_leader);
		
		add(lfamilyid, ldbfamily);
		m_rolefamily.insert_or_assign(aroleid, lfamilyid);
		sync_family(aroleid);
		return 0;
	}

	int32_t family::join_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		int64_t* lpfamilyid = tools::findmap(m_rolefamily, aroleid);
		if (lpfamilyid != nullptr)
		{
			return 1;
		}
		// Joinapplyjoinlist
		std::set<int64_t>& lset = m_applylist[aroleid];
		if (lset.contains(afamilyid))
		{
			return 2;
		}
		if (lset.size() >= ttab_specialid::instance().m_familapplylistcount)
		{
			return 3;
		}
		ngl::data_modified<pbdb::db_family>& lpmodifiedfamily = get(afamilyid);
		MODIFIED_RETURN_CONST(lpfamilyconst, lpmodifiedfamily, 4);

		int32_t* lmaxcount = ttab_familylv::instance().failylvmaxcount(lpfamilyconst->mlv());
		if (lmaxcount == nullptr)
		{
			return 5;
		}
		if (lpfamilyconst->mapplylist().size() >= *lmaxcount)
		{
			return 6;
		}
		MODIFIED_RETUAN(lpfamily, lpmodifiedfamily, 7);
		*lpfamily->mutable_mapplylist()->Add() = aroleid;
		lset.insert(afamilyid);
		return 0;
	}

	int32_t family::erase_applylist(i64_actorid aroleid, i64_actorid afamilyid)
	{
		std::set<int64_t>& lset = m_applylist[aroleid];
		lset.erase(afamilyid);
		ngl::data_modified<pbdb::db_family>& lpmodifiedfamily = get(afamilyid);
		MODIFIED_RETUAN(lpfamily, lpmodifiedfamily, 1);
		auto& applylist = *lpfamily->mutable_mapplylist();
		auto it = std::ranges::find(applylist, aroleid);
		if (it == applylist.end())
		{
			return 2;
		}
		applylist.erase(it);
		return 0;
	}

	int32_t family::cancel_join_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		return erase_applylist(aroleid, afamilyid);
	}

	int32_t family::ratify_join_family(i64_actorid aroleid, i64_actorid ajoinroleid, bool aratify)
	{
		auto itorrolefamily = m_rolefamily.find(aroleid);
		if (itorrolefamily == m_rolefamily.end())
		{
			return 1;
		}
		i64_actorid lfamilyid = itorrolefamily->second;
		std::set<int64_t>& lset = m_applylist[ajoinroleid];
		if (lset.contains(lfamilyid) == false)
		{
			return 2;
		}
		if (aratify)
		{
			ngl::data_modified<pbdb::db_family>& lpmodifiedfamily = get(lfamilyid);
			MODIFIED_RETUAN(lpfamily, lpmodifiedfamily, 3);
			lpfamily->mutable_mmember()->Add(ajoinroleid);
			for (int64_t litem : lset)
			{
				erase_applylist(ajoinroleid, litem);
			}
			lset.clear();
		}
		else
		{
			erase_applylist(ajoinroleid, lfamilyid);
		}

		lset.erase(lfamilyid);

		return 0;
	}

	int32_t family::cede_family(i64_actorid aroleid, i64_actorid acederoleid)
	{
		i64_actorid* lfamilyid = tools::findmap(m_rolefamily, aroleid);
		i64_actorid* lcedefamilyid = tools::findmap(m_rolefamily, acederoleid);
		if (lfamilyid == nullptr || lcedefamilyid == nullptr)
		{
			return 1;
		}
		if (*lfamilyid != *lcedefamilyid)
		{
			return 2;
		}

		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(*lfamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 3;
		}
		MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily, 4);
		if (lpfamilyconst->mleader() != aroleid)
		{
			return 5;
		}
		MODIFIED_RETUAN(lpfamily, *lpmodifiedfamily, 6);
		lpfamily->set_mleader(acederoleid);

		data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);
		MODIFIED_RETUAN(lpfamilyer, lpmodifiedfamilyer, 7);
		lpfamilyer->set_mposition(pbdb::db_familyer_eposition_ordinary);

		data_modified<pbdb::db_familyer>& lpmodifiedcedefamilyer = nactor()->m_familyer.get(acederoleid);
		MODIFIED_RETUAN(lpcedefamilyer, lpmodifiedcedefamilyer, 8);
		lpcedefamilyer->set_mposition(pbdb::db_familyer_eposition_leader);

		return 0;
	}

	int32_t family::leave_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		auto itorrolefamily = m_rolefamily.find(aroleid);
		if (itorrolefamily == m_rolefamily.end())
		{
			return 1;
		}
		if (itorrolefamily->second != afamilyid)
		{
			return 2;
		}
		data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);
		MODIFIED_RETUAN(lpfamilyer, lpmodifiedfamilyer, 4);
		if (lpfamilyer->mposition() == pbdb::db_familyer_eposition_leader)
		{
			return 5;
		}
		
		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(afamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 6;
		}
		MODIFIED_RETUAN(lpfamily, *lpmodifiedfamily, 7);
		auto& lmember = *lpfamily->mutable_mmember();
		auto itor = std::ranges::find(lmember, aroleid);
		if (itor == lmember.end())
		{
			return 8;
		}
		lpfamilyer->set_mposition(pbdb::db_familyer_eposition_none);
		lpfamilyer->set_mlastleaveutc((int32_t)localtime::gettime());
		lmember.erase(itor);
		return 0;
	}

	bool family::get_familylist(i64_actorid afamilyid, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& apro)
	{
		if (afamilyid == -1)
		{
			apro.set_mfamilid(afamilyid);
			for (auto& lpair : data())
			{
				MODIFIED_CONTINUE_CONST(lpfamilyconst, lpair.second);
				*apro.add_mfamily() = *lpfamilyconst;
			}
			return true;
		}
		else
		{
			ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(afamilyid);
			if (lpmodifiedfamily == nullptr)
			{
				return false;
			}
			apro.set_mfamilid(afamilyid);
			MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily, false);
			*apro.add_mfamily() = *lpfamilyconst;
			return true;
		}
	}

	void family::sync_familylist(i64_actorid aroleid, i64_actorid afamilyid)
	{
		pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE pro;
		if (!get_familylist(afamilyid, pro))
		{
			return;
		}
		actor::send_client(aroleid, pro);
	}

	void family::sync_family(i64_actorid aroleid)
	{
		pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE pro;
		pro.set_mstat(0);
		auto lpfamilyid = tools::findmap(m_rolefamily, aroleid);
		if (lpfamilyid == nullptr)
		{
			pro.set_mstat(1);
			actor::send_client(aroleid, pro);
			return;
		}
		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(*lpfamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			pro.set_mstat(2);
			actor::send_client(aroleid, pro);
			return;
		}
		MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily);
		*pro.mutable_minfo() = *lpfamilyconst;
		const i64_actorid lselfid = get_actor()->id_guid();
		for (const int64_t lmemberid : lpfamilyconst->mmember())
		{
			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cread<actor_family>::instance(lselfid).getconst(lmemberid);
			if (lpbrief != nullptr)
			{
				*pro.add_mmember() = *lpbrief;
			}
		}
		actor::send_client(aroleid, pro);
	}

	int32_t family::change_familyname(i64_actorid aroleid, i64_actorid afamilyid, const std::string& afamilyname)
	{
		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(afamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 1;
		}
		if (aroleid != -1)
		{
			MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily, 2);
			if (lpfamilyconst->mleader() != aroleid)
			{
				return 3;
			}
		}
		MODIFIED_RETUAN(lpfamily, *lpmodifiedfamily, 4);
		lpfamily->set_mname(afamilyname.c_str());
		if (aroleid != -1)
		{
			sync_family(aroleid);
		}
		return 0;
	}

	int32_t family::sign_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		int64_t* lpfamilyid = tools::findmap(m_rolefamily, aroleid);
		if (lpfamilyid == nullptr)
		{
			return 1;
		}

		if (*lpfamilyid != afamilyid)
		{
			return 2;
		}

		ngl::data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);

		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(afamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 3;
		}

		const int32_t lnow = (int32_t)localtime::gettime();
		MODIFIED_RETUAN(lpfamilyer, lpmodifiedfamilyer, 4);
		if (localtime::getspandays(lnow, lpfamilyer->mlastsignutc()) == 0)
		{
			return 4;
		}

		lpfamilyer->set_mlastsignutc(lnow);

		// Toguild
		MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily, 5);
		int32_t* lpexp = ttab_familylv::instance().failylvexp(lpfamilyconst->mlv());
		if (lpexp == nullptr)
		{
			return 5;
		}
		int32_t lexp = lpfamilyconst->mexp() + ttab_specialid::instance().m_familsignexp;
		MODIFIED_RETUAN(lpfamily, *lpmodifiedfamily, 7);
		if (*lpexp <= lexp)
		{
			lexp -= *lpexp;
			lpfamily->set_mlv(lpfamily->mlv()+1);
		}
		lpfamily->set_mexp(lexp);

		// Sendreward
		if (!actor_family::instance().get_drop().use(ttab_specialid::instance().m_familsigndrop, 1, aroleid, "family sign"))
		{
			return 6;
		}
		return 0;
	}

	bool family::get_familyers(i64_actorid aroleid, std::vector<i64_actorid>& afamilyers)
	{
		int64_t* lfamilyid = tools::findmap(m_rolefamily, aroleid);
		if (lfamilyid == nullptr)
		{
			return false;
		}
		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(*lfamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return false;
		}
		MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily, false);
		afamilyers.reserve(afamilyers.size() + lpfamilyconst->mmember_size());
		for (const int64_t afamilyer : lpfamilyconst->mmember())
		{
			if (afamilyer == aroleid)
			{
				continue;
			}
			afamilyers.emplace_back(afamilyer);
		}
		return !afamilyers.empty();
	}
}//namespace ngl
