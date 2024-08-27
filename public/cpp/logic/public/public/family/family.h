#pragma once

#include "ttab_specialid.h"
#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class familyer : public tdb_familyer::db_modular
	{
		familyer(const familyer&) = delete;
		familyer& operator=(const familyer&) = delete;

	public:
		familyer()
		{}

		virtual void set_id()
		{
			m_id = -1;
		}

		const pbdb::db_familyer* get_constfamilyer(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.getconst();
		}

		pbdb::db_familyer* get_familyer(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.get(achange);
		}

		pbdb::db_familyer* add_familyer(i64_actorid aroleid)
		{
			if (pbdb::db_familyer* lpfamilyer = get_familyer(aroleid); lpfamilyer == nullptr)
			{
				return lpfamilyer;
			}
			pbdb::db_familyer lfamilyer;
			lfamilyer.set_m_id(aroleid);
			lfamilyer.set_m_joinutc(0);
			lfamilyer.set_m_lastleaveutc(0);
			lfamilyer.set_m_lastsignutc(0);
			lfamilyer.set_m_position(pbdb::db_familyer_eposition_none);
			auto lptemp = add(aroleid, lfamilyer);
			if (lptemp == nullptr)
				return nullptr;
			return &lptemp->get();
		}

		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) << "familyer###loaddb_finish" << std::endl;
			for (const auto& [_roleid, _data] : data())
			{

			}
		}

		// 检查是否可以创建军团
		bool check_createfamily(i64_actorid aroleid)
		{
			pbdb::db_familyer* lpfamilyer = add_familyer(aroleid);
			if (lpfamilyer->m_position() != pbdb::db_familyer_eposition_none)
			{
				return false;
			}
			auto lnow = (int32_t)localtime::gettime();
			if (lnow - lpfamilyer->m_lastleaveutc() < ttab_specialid::m_familjoininterval)
			{
				return false;
			}
			return true;
		}
	};


	class family : public tdb_family::db_modular
	{
		family(const family&) = delete;
		family& operator=(const family&) = delete;

		int32_t m_maxid;
		std::map<int64_t, int64_t>				m_rolefamily;	// key:roleid value:familyid
		std::map<int64_t, std::set<int64_t>>	m_applylist;	// key:roleid value:std::set<familyid>
		std::set<std::string>					m_familyname;	// 用来检查军团名称是否重复
	public:
		family():
			m_maxid(0)
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		const pbdb::db_family* get_constfamily(i64_actorid afamilyid)
		{
			auto itor = data().find(afamilyid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.getconst();
		}

		pbdb::db_family* get_family(i64_actorid afamilyid, bool achange = true)
		{
			auto itor = data().find(afamilyid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.get(achange);
		}

		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) << "family###loaddb_finish" << std::endl;
			for (const auto& [_familyid, _family] : data())
			{
				const pbdb::db_family& lbdfamily = _family.getconst();
				if (m_maxid < _familyid)
				{
					m_maxid = _familyid;
				}
				(*lstream) <<
					std::format("id:{} name:{} createutc:{} leader:{} lv:{} exp:{}", 
						_familyid,lbdfamily.m_name(),lbdfamily.m_createutc(),
						lbdfamily.m_leader(),lbdfamily.m_lv(),lbdfamily.m_exp()
					);
				(*lstream) << " member:[" << std::endl;
				for (i64_actorid roleid : lbdfamily.m_member())
				{
					(*lstream) <<std::format("roleid:{}",roleid) << std::endl;
					m_rolefamily[roleid] = _familyid;
				}
				(*lstream) << " ]member" << std::endl;

				m_familyname.insert(lbdfamily.m_name());

				for (i64_actorid roleid : lbdfamily.m_applylist())
				{
					m_applylist[roleid].insert(_familyid);
				}
			}
			(*lstream).print("");
		}

		// # 创建军团
		int32_t create_family(i64_actorid aroleid, const std::string& aname);

		// # 申请加入军团
		int32_t join_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # 从军团[afamilyid]中的applylist中移除[aroleid]
		int32_t erase_applylist(i64_actorid aroleid, i64_actorid afamilyid);

		// # 取消加入军团
		int32_t cancel_join_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # 军团长同意/不同意玩家加入军团
		int32_t ratify_join_family(i64_actorid aroleid, i64_actorid ajoinroleid, bool aratify);

		// # 军团长转让军团
		int32_t cede_family(i64_actorid aroleid, i64_actorid acederoleid);

		// # 退出军团
		int32_t leave_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # 修改军团名称
		int32_t change_familyname(i64_actorid aroleid, i64_actorid afamilyid, const std::string& afamilyname);

		// # 军团签到
		int32_t sign_family(i64_actorid aroleid, i64_actorid afamilyid);

		std::shared_ptr<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE> get_familylist(i64_actorid afamilyid);

		void sync_family(i64_actorid aroleid, i64_actorid afamilyid);

	};




}// namespace ngl