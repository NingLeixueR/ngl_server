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
// File overview: Implements family data operations and guild state transitions.


#include "actor/actor_logic/actor_family/actor_family.h"
#include "actor/actor_logic/actor_family/family.h"
#include "actor/tab/ttab_specialid.h"
#include "actor/tab/ttab_familylv.h"
#include "actor/tab/csvtable.h"

namespace ngl
{
	/**
	 * Create a new family with the specified role as leader.
	 *
	 * @param aroleid The role ID of the player creating the family
	 * @param aname The name of the new family
	 * @return Error code: 0=success, 1=role not found, 2=cannot create family, 3=name already exists, 4=database error
	 */
	int32_t family::create_family(i64_actorid aroleid, const std::string& aname)
	{
		// Check if role exists in the role-family mapping
		if (!m_rolefamily.contains(aroleid))
		{
			return 1;
		}
		// Verify role meets requirements to create a family
		if (nactor()->m_familyer.check_createfamily(aroleid) == false)
		{
			return 2;
		}
		// Check if family name is already taken
		if (m_familyname.contains(aname))
		{
			return 3;
		}

		// Initialize new family database record
		pbdb::db_family ldbfamily;
		int32_t lnow = (int32_t)tools::time::gettime();
		i64_actorid lfamilyid = nguid::make(ACTOR_FAMILY, tab_self_area, ++m_maxid);
		ldbfamily.set_mid(lfamilyid);
		ldbfamily.set_mcreateutc(lnow);
		ldbfamily.set_mname(aname);
		ldbfamily.set_mleader(aroleid);
		ldbfamily.set_mlv(0);
		ldbfamily.set_mexp(0);
		*ldbfamily.mutable_mmember()->Add() = aroleid;

		// Set creator as family leader in familyer table
		data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);
		MODIFIED_RETUAN(lpfamilyer, lpmodifiedfamilyer, 4);
		lpfamilyer->set_mid(aroleid);
		lpfamilyer->set_mlastsignutc(0);
		lpfamilyer->set_mjoinutc(lnow);
		lpfamilyer->set_mposition(pbdb::db_familyer_eposition_leader);

		// Add family to database and update mappings
		add(lfamilyid, ldbfamily);
		m_rolefamily.insert_or_assign(aroleid, lfamilyid);
		sync_family(aroleid);
		return 0;
	}

	/**
	 * Request to join a family by adding to the family's application list.
	 *
	 * @param aroleid The role ID of the player requesting to join
	 * @param afamilyid The ID of the family to join
	 * @return Error code: 0=success, 1=already in a family, 2=already applied, 3=apply list full,
	 *         4=family not found, 5=invalid family level, 6=family application list full, 7=database error
	 */
	int32_t family::join_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		// Check if player is already in a family
		int64_t* lpfamilyid = tools::findmap(m_rolefamily, aroleid);
		if (lpfamilyid != nullptr)
		{
			return 1;
		}
		// Check if already applied to this family
		std::set<int64_t>& lset = m_applylist[aroleid];
		if (lset.contains(afamilyid))
		{
			return 2;
		}
		// Check if player's application list is full
		if (lset.size() >= ttab_specialid::instance().m_familapplylistcount)
		{
			return 3;
		}
		ngl::data_modified<pbdb::db_family>& lpmodifiedfamily = get(afamilyid);
		MODIFIED_RETURN_CONST(lpfamilyconst, lpmodifiedfamily, 4);

		// Check if family can accept more applications based on level
		int32_t* lmaxcount = ttab_familylv::instance().failylvmaxcount(lpfamilyconst->mlv());
		if (lmaxcount == nullptr)
		{
			return 5;
		}
		if (lpfamilyconst->mapplylist().size() >= *lmaxcount)
		{
			return 6;
		}
		// Add application to family's pending list
		MODIFIED_RETUAN(lpfamily, lpmodifiedfamily, 7);
		*lpfamily->mutable_mapplylist()->Add() = aroleid;
		lset.insert(afamilyid);
		return 0;
	}

	/**
	 * Remove a role's application from a family's application list.
	 *
	 * @param aroleid The role ID of the applicant
	 * @param afamilyid The ID of the family
	 * @return Error code: 0=success, 1=database error, 2=application not found
	 */
	int32_t family::erase_applylist(i64_actorid aroleid, i64_actorid afamilyid)
	{
		// Remove from player's application list
		std::set<int64_t>& lset = m_applylist[aroleid];
		lset.erase(afamilyid);
		// Remove from family's application list
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

	/**
	 * Cancel a pending family join application.
	 *
	 * @param aroleid The role ID of the applicant
	 * @param afamilyid The ID of the family
	 * @return Error code from erase_applylist
	 */
	int32_t family::cancel_join_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		return erase_applylist(aroleid, afamilyid);
	}

	/**
	 * Ratify or reject a family join application.
	 *
	 * @param aroleid The role ID of the family leader making the decision
	 * @param ajoinroleid The role ID of the applicant
	 * @param aratify True to accept, false to reject
	 * @return Error code: 0=success, 1=leader not in family, 2=no pending application, 3=database error
	 */
	int32_t family::ratify_join_family(i64_actorid aroleid, i64_actorid ajoinroleid, bool aratify)
	{
		// Verify leader is in a family
		auto itorrolefamily = m_rolefamily.find(aroleid);
		if (itorrolefamily == m_rolefamily.end())
		{
			return 1;
		}
		i64_actorid lfamilyid = itorrolefamily->second;
		// Check if applicant has applied to this family
		std::set<int64_t>& lset = m_applylist[ajoinroleid];
		if (lset.contains(lfamilyid) == false)
		{
			return 2;
		}
		if (aratify)
		{
			// Accept: add member to family
			ngl::data_modified<pbdb::db_family>& lpmodifiedfamily = get(lfamilyid);
			MODIFIED_RETUAN(lpfamily, lpmodifiedfamily, 3);
			lpfamily->mutable_mmember()->Add(ajoinroleid);
			// Clear all pending applications for this player
			for (int64_t litem : lset)
			{
				erase_applylist(ajoinroleid, litem);
			}
			lset.clear();
		}
		else
		{
			// Reject: just remove the application
			erase_applylist(ajoinroleid, lfamilyid);
		}

		lset.erase(lfamilyid);

		return 0;
	}

	/**
	 * Transfer family leadership to another member.
	 *
	 * @param aroleid The current leader's role ID
	 * @param acederoleid The new leader's role ID
	 * @return Error code: 0=success, 1=role not found, 2=not in same family, 3=family not found,
	 *         4=database error, 5=not the leader, 6-8=database errors
	 */
	int32_t family::cede_family(i64_actorid aroleid, i64_actorid acederoleid)
	{
		// Verify both roles are in families
		i64_actorid* lfamilyid = tools::findmap(m_rolefamily, aroleid);
		i64_actorid* lcedefamilyid = tools::findmap(m_rolefamily, acederoleid);
		if (lfamilyid == nullptr || lcedefamilyid == nullptr)
		{
			return 1;
		}
		// Verify both are in the same family
		if (*lfamilyid != *lcedefamilyid)
		{
			return 2;
		}

		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(*lfamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 3;
		}
		// Verify current role is the leader
		MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily, 4);
		if (lpfamilyconst->mleader() != aroleid)
		{
			return 5;
		}
		// Transfer leadership
		MODIFIED_RETUAN(lpfamily, *lpmodifiedfamily, 6);
		lpfamily->set_mleader(acederoleid);

		// Update old leader's position
		data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);
		MODIFIED_RETUAN(lpfamilyer, lpmodifiedfamilyer, 7);
		lpfamilyer->set_mposition(pbdb::db_familyer_eposition_ordinary);

		// Update new leader's position
		data_modified<pbdb::db_familyer>& lpmodifiedcedefamilyer = nactor()->m_familyer.get(acederoleid);
		MODIFIED_RETUAN(lpcedefamilyer, lpmodifiedcedefamilyer, 8);
		lpcedefamilyer->set_mposition(pbdb::db_familyer_eposition_leader);

		return 0;
	}

	/**
	 * Leave a family (only for non-leader members).
	 *
	 * @param aroleid The role ID of the member leaving
	 * @param afamilyid The ID of the family to leave
	 * @return Error code: 0=success, 1=not in a family, 2=wrong family, 4=database error,
	 *         5=leader cannot leave, 6=family not found, 7-8=database errors
	 */
	int32_t family::leave_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		// Verify role is in a family
		auto itorrolefamily = m_rolefamily.find(aroleid);
		if (itorrolefamily == m_rolefamily.end())
		{
			return 1;
		}
		// Verify it's the correct family
		if (itorrolefamily->second != afamilyid)
		{
			return 2;
		}
		data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);
		MODIFIED_RETUAN(lpfamilyer, lpmodifiedfamilyer, 4);
		// Leaders must transfer leadership before leaving
		if (lpfamilyer->mposition() == pbdb::db_familyer_eposition_leader)
		{
			return 5;
		}

		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(afamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 6;
		}
		// Remove member from family
		MODIFIED_RETUAN(lpfamily, *lpmodifiedfamily, 7);
		auto& lmember = *lpfamily->mutable_mmember();
		auto itor = std::ranges::find(lmember, aroleid);
		if (itor == lmember.end())
		{
			return 8;
		}
		// Update member status and record leave time
		lpfamilyer->set_mposition(pbdb::db_familyer_eposition_none);
		lpfamilyer->set_mlastleaveutc((int32_t)tools::time::gettime());
		lmember.erase(itor);
		return 0;
	}

	/**
	 * Get family list information.
	 *
	 * @param afamilyid The family ID to query, or -1 for all families
	 * @param apro Output parameter to store the family list response
	 * @return True if successful, false if family not found
	 */
	bool family::get_familylist(i64_actorid afamilyid, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& apro)
	{
		if (afamilyid == -1)
		{
			// Return all families
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
			// Return specific family
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

	/**
	 * Synchronize family list to a client.
	 *
	 * @param aroleid The role ID to send the list to
	 * @param afamilyid The family ID to query, or -1 for all families
	 */
	void family::sync_familylist(i64_actorid aroleid, i64_actorid afamilyid)
	{
		pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE pro;
		if (!get_familylist(afamilyid, pro))
		{
			return;
		}
		actor::send_client(aroleid, pro);
	}

	/**
	 * Synchronize family information to a client, including member details.
	 *
	 * @param aroleid The role ID to send the information to
	 */
	void family::sync_family(i64_actorid aroleid)
	{
		pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE pro;
		pro.set_mstat(0);
		// Find player's family
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
		// Add member brief information
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

	/**
	 * Change the name of a family.
	 *
	 * @param aroleid The role ID requesting the change (must be leader), or -1 for system change
	 * @param afamilyid The ID of the family
	 * @param afamilyname The new family name
	 * @return Error code: 0=success, 1=family not found, 2=database error, 3=not the leader, 4=database error
	 */
	int32_t family::change_familyname(i64_actorid aroleid, i64_actorid afamilyid, const std::string& afamilyname)
	{
		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(afamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 1;
		}
		// Verify leadership if not a system change
		if (aroleid != -1)
		{
			MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily, 2);
			if (lpfamilyconst->mleader() != aroleid)
			{
				return 3;
			}
		}
		// Update family name
		MODIFIED_RETUAN(lpfamily, *lpmodifiedfamily, 4);
		lpfamily->set_mname(afamilyname.c_str());
		if (aroleid != -1)
		{
			sync_family(aroleid);
		}
		return 0;
	}

	/**
	 * Daily family sign-in to gain experience and rewards.
	 *
	 * @param aroleid The role ID signing in
	 * @param afamilyid The ID of the family
	 * @return Error code: 0=success, 1=not in a family, 2=wrong family, 3=family not found,
	 *         4=already signed today, 5=invalid family level, 7=database error, 6=reward distribution failed
	 */
	int32_t family::sign_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		// Verify player is in the correct family
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

		const int32_t lnow = (int32_t)tools::time::gettime();
		MODIFIED_RETUAN(lpfamilyer, lpmodifiedfamilyer, 4);
		// Check if already signed in today
		if (tools::time::getspandays(lnow, lpfamilyer->mlastsignutc()) == 0)
		{
			return 4;
		}

		lpfamilyer->set_mlastsignutc(lnow);

		// Add experience to family
		MODIFIED_RETURN_CONST(lpfamilyconst, *lpmodifiedfamily, 5);
		int32_t* lpexp = ttab_familylv::instance().failylvexp(lpfamilyconst->mlv());
		if (lpexp == nullptr)
		{
			return 5;
		}
		int32_t lexp = lpfamilyconst->mexp() + ttab_specialid::instance().m_familsignexp;
		MODIFIED_RETUAN(lpfamily, *lpmodifiedfamily, 7);
		// Level up if enough experience
		if (*lpexp <= lexp)
		{
			lexp -= *lpexp;
			lpfamily->set_mlv(lpfamily->mlv()+1);
		}
		lpfamily->set_mexp(lexp);

		// Send sign-in reward to player
		if (!actor_family::instance().get_drop().use(ttab_specialid::instance().m_familsigndrop, 1, aroleid, "family sign"))
		{
			return 6;
		}
		return 0;
	}

	/**
	 * Get all family members for a given role.
	 *
	 * @param aroleid The role ID to query
	 * @param afamilyers Output vector to store family member IDs (excluding the querying role)
	 * @return True if successful and family has other members, false otherwise
	 */
	bool family::get_familyers(i64_actorid aroleid, std::vector<i64_actorid>& afamilyers)
	{
		// Find player's family
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
		// Collect all family members except the querying role
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
