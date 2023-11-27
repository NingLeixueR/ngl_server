#pragma once

#include "actor.h"
#include "map.h"


namespace ngl
{
	class actor_plays : public actor
	{
		tab_plays* m_tab;
		aoimap m_map;
	public:
		actor_plays(int32_t adataid);

		virtual void init()
		{
		}

		static void actor_register();

		virtual ~actor_plays() {}

		virtual void loaddb_finish(bool adbishave) {}

		enum { ACTOR_TYPE = ACTOR_PLAYS };

		template <typename T>
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, T& adata)
		{
			return true;
		}

		bool timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
		{
			return true;
		}
	};
}