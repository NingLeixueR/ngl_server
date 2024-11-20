#include "ttab_specialid.h"
#include "ttab_familylv.h"
#include "actor_family.h"
#include "csvtable.h"
#include "family.h"

namespace ngl
{
	int32_t family::create_family(i64_actorid aroleid, const std::string& aname)
	{
		if (m_rolefamily.find(aroleid) != m_rolefamily.end())
			return 1;
		if (actor()->m_familyer.check_createfamily(aroleid) == false)
			return 2;
		if (m_familyname.contains(aname))
			return 3;
		pbdb::db_family ldbfamily;
		++m_maxid;

		int32_t lnow = localtime::gettime();
		i64_actorid lfamilyid = nguid::make(ACTOR_FAMILY, tab_self_area, m_maxid);
		ldbfamily.set_m_id(lfamilyid);
		ldbfamily.set_m_createutc(lnow);
		ldbfamily.set_m_name(aname);
		ldbfamily.set_m_leader(aroleid);
		ldbfamily.set_m_lv(0);
		ldbfamily.set_m_exp(0);
		*ldbfamily.mutable_m_member()->Add() = aroleid;

		pbdb::db_familyer* lpfamilyer = actor()->m_familyer.add_familyer(aroleid);
		if (lpfamilyer == nullptr)
		{
			return 3;
		}

		lpfamilyer->set_m_joinutc(lnow);
		lpfamilyer->set_m_lastsignutc(0);
		lpfamilyer->set_m_position(pbdb::db_familyer_eposition_leader);
		lpfamilyer->set_m_id(aroleid);
		
		add(lfamilyid, ldbfamily);
		m_rolefamily[aroleid] = lfamilyid;
		sync_family(aroleid, lfamilyid);
		return 0;
	}

	int32_t family::join_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		int64_t* lpfamilyid = tools::findmap(m_rolefamily, aroleid);
		if (lpfamilyid != nullptr)
		{
			return 1;
		}
		// 加入申请加入列表
		std::set<int64_t>& lset = m_applylist[aroleid];
		if (lset.contains(afamilyid))
		{
			return 2;
		}
		if (lset.size() >= ttab_specialid::m_familapplylistcount)
		{
			return 3;
		}
		const pbdb::db_family* lpconstfamily = get_constfamily(afamilyid);
		if (lpconstfamily == nullptr)
		{
			return 4;
		}
		int32_t* lmaxcount = ttab_familylv::failylvmaxcount(lpconstfamily->m_lv());
		if (lmaxcount == nullptr)
		{
			return 5;
		}
		if (lpconstfamily->m_applylist().size() >= *lmaxcount)
		{
			return 6;
		}

		pbdb::db_family* lpfamily = get_family(afamilyid);
		if (lpfamily == nullptr)
		{
			return 7;
		}
		*lpfamily->mutable_m_applylist()->Add() = aroleid;
		lset.insert(afamilyid);
		return 0;
	}

	int32_t family::erase_applylist(i64_actorid aroleid, i64_actorid afamilyid)
	{
		std::set<int64_t>& lset = m_applylist[aroleid];
		lset.erase(afamilyid);
		pbdb::db_family* lpfamily = get_family(afamilyid);
		if (lpfamily == nullptr)
		{
			return 1;
		}
		auto& applylist = *lpfamily->mutable_m_applylist();
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
			pbdb::db_family* lpfamily = get_family(lfamilyid);
			if (lpfamily == nullptr)
			{
				return 1;
			}

			lpfamily->mutable_m_member()->Add(ajoinroleid);
			for (int64_t item : lset)
			{
				erase_applylist(ajoinroleid, item);
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

		const pbdb::db_family* lpconstfamily = get_constfamily(*lfamilyid);
		if (lpconstfamily == nullptr)
		{
			return 3;
		}
		if (lpconstfamily->m_leader() != aroleid)
		{
			return 4;
		}

		pbdb::db_family* lpfamily = get_family(*lfamilyid);
		if (lpfamily == nullptr)
		{
			return 5;
		}
		lpfamily->set_m_leader(acederoleid);

		pbdb::db_familyer* lpfamilyer = actor()->m_familyer.add_familyer(aroleid);
		if (lpfamilyer == nullptr)
		{
			return 6;
		}
		lpfamilyer->set_m_position(pbdb::db_familyer_eposition_ordinary);

		pbdb::db_familyer* lpcedefamilyer = actor()->m_familyer.add_familyer(acederoleid);
		if (lpcedefamilyer == nullptr)
		{
			return 7;
		}
		lpcedefamilyer->set_m_position(pbdb::db_familyer_eposition_leader);

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
		pbdb::db_familyer* lpfamilyer = actor()->m_familyer.add_familyer(aroleid);
		if (lpfamilyer == nullptr)
		{
			return 3;
		}
		if (lpfamilyer->m_position() == pbdb::db_familyer_eposition_leader)
		{
			return 4;
		}
		
		pbdb::db_family* lpfamily = get_family(afamilyid);
		if (lpfamily == nullptr)
		{
			return 5;
		}
		auto& lmember = *lpfamily->mutable_m_member();
		auto itor = std::ranges::find(lmember, aroleid);
		if (itor == lmember.end())
		{
			return 6;
		}
		lpfamilyer->set_m_position(pbdb::db_familyer_eposition_none);
		lpfamilyer->set_m_lastleaveutc(localtime::gettime());
		lmember.erase(itor);
		return 0;
	}

	std::shared_ptr<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE> family::get_familylist(i64_actorid afamilyid)
	{
		if (afamilyid == -1)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>();
			pro->set_m_familid(afamilyid);
			for (const auto& [_familid, _famil] : data())
			{
				*pro->add_m_family() = _famil.getconst();
			}
			return pro;
		}
		else
		{
			const pbdb::db_family* lpconstfamily = get_constfamily(afamilyid);
			if (lpconstfamily == nullptr)
			{
				return nullptr;
			}
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>();
			pro->set_m_familid(afamilyid);
			*pro->add_m_family() = *lpconstfamily;
			return pro;
		}
	}

	void family::sync_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		auto pro = get_familylist(afamilyid);
		actor::send_client(aroleid, pro);
	}

	int32_t family::change_familyname(i64_actorid aroleid, i64_actorid afamilyid, const std::string& afamilyname)
	{
		const pbdb::db_family* lpconstfamily = get_constfamily(afamilyid);
		if (lpconstfamily == nullptr)
		{
			return 1;
		}
		if (aroleid != -1)
		{
			if (lpconstfamily->m_leader() != aroleid)
				return 2;
		}
		pbdb::db_family* lpfamily = get_family(afamilyid);
		if (lpfamily == nullptr)
		{
			return 3;
		}
		lpfamily->set_m_name(afamilyname.c_str());
		if (aroleid != -1)
		{
			sync_family(aroleid, afamilyid);
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

		pbdb::db_familyer* lpfamilyer = actor()->m_familyer.add_familyer(aroleid);
		if (lpfamilyer == nullptr)
		{
			return 3;
		}

		pbdb::db_family* lpfamily = get_family(afamilyid);
		if (lpfamily == nullptr)
		{
			return 4;
		}

		auto lnow = (int32_t)localtime::gettime();
		if (localtime::getspandays(lnow, lpfamilyer->m_lastsignutc()) == 0)
		{
			return 5;
		}

		lpfamilyer->set_m_lastsignutc(localtime::gettime());

		// 给军团增加经验
		int32_t* lpexp = ttab_familylv::failylvexp(lpfamily->m_lv());
		if (lpexp == nullptr)
		{
			return 6;
		}
		int32_t lexp = lpfamily->m_exp() + ttab_specialid::m_familsignexp;
		if (*lpexp <= lexp)
		{
			lexp -= *lpexp;
			lpfamily->set_m_lv(lpfamily->m_lv()+1);
		}
		lpfamily->set_m_exp(lexp);

		// 发送奖励
		if (drop::use(aroleid, ttab_specialid::m_familsigndrop, 1, afamilyid) == false)
		{
			return 7;
		}
		return 0;
	}

	bool family::get_familyers(i64_actorid aroleid, std::vector<i64_actorid>& afamilyers)
	{
		int64_t* lfamilyid = tools::findmap(m_rolefamily, aroleid);
		if (lfamilyid == nullptr)
			return false;
		pbdb::db_family* lpfamily = get_family(*lfamilyid);
		if (lpfamily == nullptr)
			return false;
		std::ranges::for_each(lpfamily->m_member(), [&afamilyers, aroleid](const int64_t afamilyer)
			{
				if (afamilyer == aroleid)
					return;
				afamilyers.push_back(afamilyer);
			});
		return !afamilyers.empty();
	}
}//namespace ngl