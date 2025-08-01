#pragma once

#include "ttab_specialid.h"
#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

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

		// 检查是否可以创建军团
		bool check_createfamily(i64_actorid aroleid)
		{
			data_modified<pbdb::db_familyer>& lpdbfamilyer = get(aroleid);
			auto lnow = (int32_t)localtime::gettime();
			if (lnow - lpdbfamilyer.getconst().mlastleaveutc() < ttab_specialid::instance().m_familjoininterval)
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
		std::set<std::string>					m_familyname;	// 用来检查军团名称是否重复
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
			for (std::pair<const nguid, data_modified<pbdb::db_family>>& lpair : data())
			{
				const pbdb::db_family& lbdfamily = lpair.second.getconst();
				m_maxid = std::max(m_maxid, (int32_t)lpair.first.id());
				
				std::string lmember;
				for (i64_actorid roleid : lbdfamily.mmember())
				{
					m_rolefamily[roleid] = lpair.first;
				}

				m_familyname.insert(lbdfamily.mname());

				for (i64_actorid roleid : lbdfamily.mapplylist())
				{
					m_applylist[roleid].insert(lpair.first);
				}
			}
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

		void sync_familylist(i64_actorid aroleid, i64_actorid afamilyid);

		void sync_family(i64_actorid aroleid);

		// # 获取军团所有人
		bool get_familyers(i64_actorid aroleid, std::vector<i64_actorid>& afamilyers);
	};
}// namespace ngl

mk_formatter(pbdb::db_familyer)
mk_formatter(pbdb::db_family)