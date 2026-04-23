#pragma once

#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ntimer.h"
#include "actor/protocol/nprotocol.h"
#include "tools/ai/nbt.h"

namespace ngl
{
	class actor_testai :
		public actor
	{
		actor_testai(const actor_testai&) = delete;
		actor_testai& operator=(const actor_testai&) = delete;

		static nbt_factory<actor_testai>	m_factory;
		nbt_context<actor_testai>			m_bt;
		int32_t m_patrol_index = 0;
		int32_t m_hp = 100;
	public:
		actor_testai(i16_area aarea, i32_actordataid aid);

		virtual ~actor_testai() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(int32_t adataid);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);
	};
}// namespace ngl
