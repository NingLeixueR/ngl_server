#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"
#include "actor.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_brief;

	class briefdb : public tdb_brief::db_modular
	{
		briefdb(const briefdb&) = delete;
		briefdb& operator=(const briefdb&) = delete;

	public:
		briefdb()
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		const pbdb::db_brief* get_constbrief(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second.getconst();
		}

		pbdb::db_brief* get_brief(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second.get(achange);
		}

		// 没有就添加
		void update_brief(const pbdb::db_brief* adata)
		{
			data()[adata->m_id()].get() = *adata;
		}

		virtual void initdata()
		{
			log_error()->print("actor_brief###loaddb_finish\n {}", data());
		}

		void update(const std::vector<pbdb::db_brief>& m_vecinfo)
		{
			for (const pbdb::db_brief& item : m_vecinfo)
			{
				update_brief(&item);
			}
		}
	};
}// namespace ngl

template <>
struct std::formatter<ngl::data_modified<pbdb::db_brief>>
{
	constexpr auto parse(const std::format_parse_context& ctx)const
	{
		return ctx.begin();
	}

	auto format(const ngl::data_modified<pbdb::db_brief>& aval, std::format_context& ctx)const
	{
		const auto& lbrief = aval.getconst();
		return std::format_to(ctx.out(),
			"<m_id={}:m_lv={}:m_moneygold={}:m_moneysilver={}:m_name={}:m_vip={}>\n",
			lbrief.m_id(), lbrief.m_lv(), lbrief.m_moneygold(), lbrief.m_moneysilver(), lbrief.m_name(), lbrief.m_vip()
		);
	}
};
