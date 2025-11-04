/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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
		ldbfamily.set_mid(lfamilyid);
		ldbfamily.set_mcreateutc(lnow);
		ldbfamily.set_mname(aname);
		ldbfamily.set_mleader(aroleid);
		ldbfamily.set_mlv(0);
		ldbfamily.set_mexp(0);
		*ldbfamily.mutable_mmember()->Add() = aroleid;

		data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);
		data_modified_return_get(lpfamilyer, lpmodifiedfamilyer, 4);
		lpfamilyer->set_mid(aroleid);
		lpfamilyer->set_mlastsignutc(0);
		lpfamilyer->set_mjoinutc(lnow);
		lpfamilyer->set_mposition(pbdb::db_familyer_eposition_leader);
		
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
		ngl::data_modified<pbdb::db_family>& lpmodifiedfamily = get(afamilyid);
		data_modified_return_getconst(lpfamilyconst, lpmodifiedfamily, 4);

		int32_t* lmaxcount = ttab_familylv::instance().failylvmaxcount(lpfamilyconst->mlv());
		if (lmaxcount == nullptr)
		{
			return 5;
		}
		if (lpfamilyconst->mapplylist().size() >= *lmaxcount)
		{
			return 6;
		}
		data_modified_return_get(lpfamily, lpmodifiedfamily, 7);
		*lpfamily->mutable_mapplylist()->Add() = aroleid;
		lset.insert(afamilyid);
		return 0;
	}

	int32_t family::erase_applylist(i64_actorid aroleid, i64_actorid afamilyid)
	{
		std::set<int64_t>& lset = m_applylist[aroleid];
		lset.erase(afamilyid);
		ngl::data_modified<pbdb::db_family>& lpmodifiedfamily = get(afamilyid);
		data_modified_return_get(lpfamily, lpmodifiedfamily, 1);
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
			data_modified_return_get(lpfamily, lpmodifiedfamily, 3);
			lpfamily->mutable_mmember()->Add(ajoinroleid);
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

		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(*lfamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 3;
		}
		data_modified_return_getconst(lpfamilyconst, *lpmodifiedfamily, 4);
		if (lpfamilyconst->mleader() != aroleid)
		{
			return 5;
		}
		data_modified_return_get(lpfamily, *lpmodifiedfamily, 6);
		lpfamily->set_mleader(acederoleid);

		data_modified<pbdb::db_familyer>& lpmodifiedfamilyer = nactor()->m_familyer.get(aroleid);
		data_modified_return_get(lpfamilyer, lpmodifiedfamilyer, 7);
		lpfamilyer->set_mposition(pbdb::db_familyer_eposition_ordinary);

		data_modified<pbdb::db_familyer>& lpmodifiedcedefamilyer = nactor()->m_familyer.get(acederoleid);
		data_modified_return_get(lpcedefamilyer, lpmodifiedcedefamilyer, 8);
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
		data_modified_return_get(lpfamilyer, lpmodifiedfamilyer, 4);
		if (lpfamilyer->mposition() == pbdb::db_familyer_eposition_leader)
		{
			return 5;
		}
		
		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(afamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return 6;
		}
		data_modified_return_get(lpfamily, *lpmodifiedfamily, 7);
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

	std::shared_ptr<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE> family::get_familylist(i64_actorid afamilyid)
	{
		if (afamilyid == -1)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>();
			pro->set_mfamilid(afamilyid);
			for (const auto& lpair : data())
			{
				data_modified_continue_getconst(lpfamilyconst, lpair.second);
				*pro->add_mfamily() = *lpfamilyconst;
			}
			return pro;
		}
		else
		{
			ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(afamilyid);
			if (lpmodifiedfamily == nullptr)
			{
				return nullptr;
			}
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>();
			pro->set_mfamilid(afamilyid);
			data_modified_return_getconst(lpfamilyconst, *lpmodifiedfamily, nullptr);
			*pro->add_mfamily() = *lpfamilyconst;
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
		pro->set_mstat(0);
		auto itor = m_rolefamily.find(aroleid);
		if (itor == m_rolefamily.end())
		{
			pro->set_mstat(1);
			actor::send_client(aroleid, pro);
			return;
		}
		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(itor->second);
		if (lpmodifiedfamily == nullptr)
		{
			pro->set_mstat(2);
			actor::send_client(aroleid, pro);
			return;
		}
		data_modified_return_getconst(lpfamilyconst, *lpmodifiedfamily);
		*pro->mutable_minfo() = *lpfamilyconst;
		for (int64_t aroleid : lpfamilyconst->mmember())
		{
			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cread<actor_family>::instance(get_actor()->id_guid()).getconst(aroleid);
			if (lpbrief != nullptr)
			{
				*pro->add_mmember() = *lpbrief;
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
			data_modified_return_getconst(lpfamilyconst, *lpmodifiedfamily, 2);
			if (lpfamilyconst->mleader() != aroleid)
			{
				return 3;
			}
		}
		data_modified_return_get(lpfamily, *lpmodifiedfamily, 4);
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

		auto lnow = (int32_t)localtime::gettime();
		data_modified_return_get(lpfamilyer, lpmodifiedfamilyer, 4);
		if (localtime::getspandays(lnow, lpfamilyer->mlastsignutc()) == 0)
		{
			return 4;
		}

		lpfamilyer->set_mlastsignutc((int32_t)localtime::gettime());

		// 给军团增加经验
		data_modified_return_getconst(lpfamilyconst, *lpmodifiedfamily, 5);
		int32_t* lpexp = ttab_familylv::instance().failylvexp(lpfamilyconst->mlv());
		if (lpexp == nullptr)
		{
			return 5;
		}
		int32_t lexp = lpfamilyconst->mexp() + ttab_specialid::instance().m_familsignexp;
		data_modified_return_get(lpfamily, *lpmodifiedfamily, 7);
		if (*lpexp <= lexp)
		{
			lexp -= *lpexp;
			lpfamily->set_mlv(lpfamily->mlv()+1);
		}
		lpfamily->set_mexp(lexp);

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
		ngl::data_modified<pbdb::db_family>* lpmodifiedfamily = find(*lfamilyid);
		if (lpmodifiedfamily == nullptr)
		{
			return false;
		}
		data_modified_return_getconst(lpfamilyconst, *lpmodifiedfamily, false);
		for (const int64_t afamilyer : lpfamilyconst->mmember())
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