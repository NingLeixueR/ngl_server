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
		ldbfamily.set_m_id(lfamilyid);
		ldbfamily.set_m_createutc(lnow);
		ldbfamily.set_m_name(aname);
		ldbfamily.set_m_leader(aroleid);
		ldbfamily.set_m_lv(0);
		ldbfamily.set_m_exp(0);
		*ldbfamily.mutable_m_member()->Add() = aroleid;

		data_modified<pbdb::db_familyer>& lpfamilyer = nactor()->m_familyer.get(aroleid);
		pbdb::db_familyer& lfamilyer = lpfamilyer.get();
		lfamilyer.set_m_id(aroleid);
		lfamilyer.set_m_lastsignutc(0);
		lfamilyer.set_m_joinutc(lnow);
		lfamilyer.set_m_position(pbdb::db_familyer_eposition_leader);
		
		add(lfamilyid, ldbfamily);
		m_rolefamily[aroleid] = lfamilyid;
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
		// 加入申请加入列表
		std::set<int64_t>& lset = m_applylist[aroleid];
		if (lset.contains(afamilyid))
		{
			return 2;
		}
		if (lset.size() >= ttab_specialid::instance().m_familapplylistcount)
		{
			return 3;
		}
		ngl::data_modified<pbdb::db_family>& lpfamily = get(afamilyid);
		int32_t* lmaxcount = ttab_familylv::instance().failylvmaxcount(lpfamily.getconst().m_lv());
		if (lmaxcount == nullptr)
		{
			return 4;
		}
		if (lpfamily.getconst().m_applylist().size() >= *lmaxcount)
		{
			return 5;
		}

		*lpfamily.get().mutable_m_applylist()->Add() = aroleid;
		lset.insert(afamilyid);
		return 0;
	}

	int32_t family::erase_applylist(i64_actorid aroleid, i64_actorid afamilyid)
	{
		std::set<int64_t>& lset = m_applylist[aroleid];
		lset.erase(afamilyid);
		ngl::data_modified<pbdb::db_family>& lpfamily = get(afamilyid);
		auto& applylist = *lpfamily.get().mutable_m_applylist();
		auto it = std::ranges::find(applylist, aroleid);
		if (it == applylist.end())
		{
			return 1;
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
			ngl::data_modified<pbdb::db_family>& lpfamily = get(lfamilyid);
			lpfamily.get().mutable_m_member()->Add(ajoinroleid);
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

		ngl::data_modified<pbdb::db_family>* lpfamily = find(*lfamilyid);
		if (lpfamily == nullptr)
		{
			return 3;
		}
		if (lpfamily->getconst().m_leader() != aroleid)
		{
			return 4;
		}

		lpfamily->get().set_m_leader(acederoleid);

		data_modified<pbdb::db_familyer>& lpfamilyer = nactor()->m_familyer.get(aroleid);
		lpfamilyer.get().set_m_position(pbdb::db_familyer_eposition_ordinary);

		data_modified<pbdb::db_familyer>& lpcedefamilyer = nactor()->m_familyer.get(acederoleid);
		lpcedefamilyer.get().set_m_position(pbdb::db_familyer_eposition_leader);

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
		data_modified<pbdb::db_familyer>& lpfamilyer = nactor()->m_familyer.get(aroleid);
		if (lpfamilyer.get().m_position() == pbdb::db_familyer_eposition_leader)
		{
			return 4;
		}
		
		ngl::data_modified<pbdb::db_family>* lpfamily = find(afamilyid);
		if (lpfamily == nullptr)
		{
			return 5;
		}
		auto& lmember = *lpfamily->get().mutable_m_member();
		auto itor = std::ranges::find(lmember, aroleid);
		if (itor == lmember.end())
		{
			return 6;
		}
		lpfamilyer.get().set_m_position(pbdb::db_familyer_eposition_none);
		lpfamilyer.get().set_m_lastleaveutc((int32_t)localtime::gettime());
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
			ngl::data_modified<pbdb::db_family>* lpfamily = find(afamilyid);
			if (lpfamily == nullptr)
			{
				return nullptr;
			}
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>();
			pro->set_m_familid(afamilyid);
			*pro->add_m_family() = lpfamily->getconst();
			return pro;
		}
	}

	void family::sync_familylist(i64_actorid aroleid, i64_actorid afamilyid)
	{
		auto pro = get_familylist(afamilyid);
		actor::send_client(aroleid, pro);
	}

	void family::sync_family(i64_actorid aroleid)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>();
		pro->set_m_stat(0);
		auto itor = m_rolefamily.find(aroleid);
		if (itor == m_rolefamily.end())
		{
			pro->set_m_stat(1);
			actor::send_client(aroleid, pro);
			return;
		}
		ngl::data_modified<pbdb::db_family>* lpfamily = find(itor->second);
		if (lpfamily == nullptr)
		{
			pro->set_m_stat(2);
			actor::send_client(aroleid, pro);
			return;
		}
		const auto& lfamily = lpfamily->getconst();
		*pro->mutable_m_info() = lfamily;
		for (int64_t aroleid : lfamily.m_member())
		{
			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_family>::instance(get_actor()->id_guid()).getconst(aroleid);
			if (lpbrief != nullptr)
			{
				*pro->add_m_member() = *lpbrief;
			}
		}
		actor::send_client(aroleid, pro);
	}

	int32_t family::change_familyname(i64_actorid aroleid, i64_actorid afamilyid, const std::string& afamilyname)
	{
		ngl::data_modified<pbdb::db_family>* lpfamily = find(afamilyid);
		if (lpfamily == nullptr)
		{
			return 1;
		}
		if (aroleid != -1)
		{
			if (lpfamily->getconst().m_leader() != aroleid)
			{
				return 2;
			}
		}
		lpfamily->get().set_m_name(afamilyname.c_str());
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

		ngl::data_modified<pbdb::db_familyer>& lpfamilyer = nactor()->m_familyer.get(aroleid);

		ngl::data_modified<pbdb::db_family>* lpfamily = find(afamilyid);
		if (lpfamily == nullptr)
		{
			return 3;
		}

		auto lnow = (int32_t)localtime::gettime();
		if (localtime::getspandays(lnow, lpfamilyer.getconst().m_lastsignutc()) == 0)
		{
			return 4;
		}

		lpfamilyer.get().set_m_lastsignutc((int32_t)localtime::gettime());

		// 给军团增加经验
		int32_t* lpexp = ttab_familylv::instance().failylvexp(lpfamily->getconst().m_lv());
		if (lpexp == nullptr)
		{
			return 5;
		}
		int32_t lexp = lpfamily->getconst().m_exp() + ttab_specialid::instance().m_familsignexp;
		if (*lpexp <= lexp)
		{
			lexp -= *lpexp;
			lpfamily->get().set_m_lv(lpfamily->get().m_lv()+1);
		}
		lpfamily->get().set_m_exp(lexp);

		// 发送奖励
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
		ngl::data_modified<pbdb::db_family>* lpfamily = find(*lfamilyid);
		if (lpfamily == nullptr)
		{
			return false;
		}
		for (const int64_t afamilyer : lpfamily->getconst().m_member())
		{
			if (afamilyer == aroleid)
			{
				continue;
			}
			afamilyers.push_back(afamilyer);
		}
		return !afamilyers.empty();
	}
}//namespace ngl