#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"
#include "actor.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_keyvalue;

	class keyvaluedb : public tdb_keyvalue::db_modular
	{
		keyvaluedb(const keyvaluedb&) = delete;
		keyvaluedb& operator=(const keyvaluedb&) = delete;

	public:
		keyvaluedb()
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		data_modified<pbdb::db_keyvalue>* get_valuedb(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		// 没有就添加
		void update_keyvalue(const pbdb::db_keyvalue* adata)
		{
			data()[adata->m_id()].get() = *adata;
		}

		virtual void initdata()
		{
			log_error()->print("actor_keyvalue###loaddb_finish {}", data());
		}

		void update(const std::vector<pbdb::db_keyvalue>& m_vecinfo)
		{
			for (const pbdb::db_keyvalue& item : m_vecinfo)
			{
				update_keyvalue(&item);
			}
		}
	};
}// namespace ngl

//ngl::data_modified<pbdb::db_keyvalue>
template <>
struct std::formatter<ngl::data_modified<pbdb::db_keyvalue>>
{
	constexpr auto parse(const std::format_parse_context& ctx)const
	{
		return ctx.begin();
	}

	auto format(const ngl::data_modified<pbdb::db_keyvalue>& aval, std::format_context& ctx)const
	{
		const auto& lkeyvalue = aval.getconst();
		return std::format_to(ctx.out(),
			"pbdb::db_keyvalue:<m_id={},m_value={}>\n",
			lkeyvalue.m_id(), lkeyvalue.m_value()
		);
	}
};