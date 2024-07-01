#include "actor_family.h"
#include "csvtable.h"
#include "family.h"

namespace ngl
{
	int32_t family::create_family(i64_actorid aroleid, const std::string& aname)
	{
		if (m_rolefamily.find(aroleid) != m_rolefamily.end())
			return 1;
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
		pbdb::familyer lfamilyer;
		lfamilyer.set_m_joinutc(lnow);
		lfamilyer.set_m_lastsignutc(0);
		lfamilyer.set_m_position(pbdb::familyer_eposition_leader);
		lfamilyer.set_m_roleid(aroleid);
		(*ldbfamily.mutable_m_member())[aroleid] = lfamilyer;
		add(lfamilyid, ldbfamily);
		m_rolefamily[aroleid] = lfamilyid;
		sync_family(aroleid, lfamilyid);
		return 0;
	}

	int32_t family::join_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		if (m_rolefamily.find(aroleid) != m_rolefamily.end())
			return 1;
		const pbdb::db_family* lpconstfamily = get_constfamily(afamilyid);
		if (lpconstfamily == nullptr)
		{
			return 2;
		}

		tab_familylv* tab = allcsv::tab<tab_familylv>(lpconstfamily->m_lv());
		if (tab == nullptr)
		{
			return 3;
		}
		if (lpconstfamily->m_member().size() >= tab->m_maxmembers)
		{
			return 4;
		}
		pbdb::db_family* lpfamily = get_family(afamilyid);
		if (lpfamily == nullptr)
		{
			return 5;
		}
		pbdb::familyer lfamilyer;
		int32_t lnow = localtime::gettime();
		lfamilyer.set_m_joinutc(lnow);
		lfamilyer.set_m_lastsignutc(0);
		lfamilyer.set_m_position(pbdb::familyer_eposition_leader);
		lfamilyer.set_m_roleid(aroleid);
		(*lpfamily->mutable_m_member())[aroleid] = lfamilyer;

		m_rolefamily[aroleid] = afamilyid;
		sync_family(aroleid, lpconstfamily->m_id());
		return 0;
	}

	int32_t family::leave_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		auto itorrolefamily = m_rolefamily.find(aroleid);
		if (itorrolefamily == m_rolefamily.end())
			return 1;
		if (itorrolefamily->second != afamilyid)
			return 2;
		const pbdb::db_family* lpconstfamily = get_constfamily(afamilyid);
		if (lpconstfamily == nullptr)
		{
			return 3;
		}
		auto itor = lpconstfamily->m_member().find(aroleid);
		if (itor == lpconstfamily->m_member().end())
		{
			return 4;
		}
		if (itor->second.m_position() == pbdb::familyer_eposition_leader)
		{
			return 5;
		}
		pbdb::db_family* lpfamily = get_family(afamilyid);
		if (lpfamily == nullptr)
		{
			return 6;
		}
		lpfamily->mutable_m_member()->erase(aroleid);
		m_rolefamily.erase(aroleid);
		return 0;
	}

	void family::sync_family(i64_actorid aroleid, i64_actorid afamilyid)
	{
		if (afamilyid == -1)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>();
			pro->set_m_familid(afamilyid);
			for (const auto& [_familid, _famil] : data())
			{
				*pro->add_m_family() = _famil.getconst();
			}
			actor::send_client(aroleid, pro);
		}
		else
		{
			const pbdb::db_family* lpconstfamily = get_constfamily(afamilyid);
			if (lpconstfamily == nullptr)
			{
				return;
			}
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>();
			pro->set_m_familid(afamilyid);
			*pro->add_m_family() = *lpconstfamily;
			actor::send_client(aroleid, pro);
		}		
	}

	void family::change_familyname(i64_actorid aroleid, i64_actorid afamilyid, const std::string& afamilyname)
	{
		const pbdb::db_family* lpconstfamily = get_constfamily(afamilyid);
		if (lpconstfamily == nullptr)
		{
			return;
		}
		if (aroleid != -1)
		{
			if (lpconstfamily->m_leader() != aroleid)
				return;
		}
		pbdb::db_family* lpfamily = get_family(afamilyid);
		if (lpfamily == nullptr)
		{
			return;
		}
		lpfamily->set_m_name(afamilyname.c_str());
		if (aroleid != -1)
		{
			sync_family(aroleid, afamilyid);
		}
	}

}//namespace ngl