#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class family : public tdb_family::db_modular
	{
		family(const family&) = delete;
		family& operator=(const family&) = delete;

		int32_t m_maxid;
		std::map<int64_t, int64_t> m_rolefamily;
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
			return &itor->second.get();
		}

		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) << "actor_family###loaddb_finish" << std::endl;
			for (const auto& [_familyid, _family] : data())
			{
				const pbdb::db_family& lbdfamily = _family.getconst();
				if (m_maxid < _familyid)
				{
					m_maxid = _familyid;
				}
				(*lstream) <<
					std::format(
						"id:{} name:{} createutc:{} leader:{} lv:{} exp:{}", 
						_familyid,
						lbdfamily.m_name(),
						lbdfamily.m_createutc(),
						lbdfamily.m_leader(),
						lbdfamily.m_lv(),
						lbdfamily.m_exp()
					);
				(*lstream) << " member:[" << std::endl;
				for (const auto& [_roleid, _familyer] : lbdfamily.m_member())
				{
					(*lstream) <<
						std::format(
							"roleid:{} joinutc:{} lastsignutc:{} position:{}",
							_roleid,
							_familyer.m_joinutc(),
							_familyer.m_lastsignutc(),
							(int)_familyer.m_position()
							) << std::endl;
					m_rolefamily[_familyer.m_roleid()] = _familyid;
				}
				(*lstream) << " ]member" << std::endl;				
			}
			(*lstream).print("");
		}

		// # 创建军团
		int32_t create_family(i64_actorid aroleid, const std::string& aname);

		// # 加入军团
		int32_t join_family(i64_actorid aroleid, i64_actorid afamilyid);

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