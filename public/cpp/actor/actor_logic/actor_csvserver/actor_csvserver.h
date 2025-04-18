#pragma once

#include "actor.h"

namespace ngl
{
	class actor_csvserver
		: public actor
	{
		actor_csvserver(const actor_csvserver&) = delete;
		actor_csvserver& operator=(const actor_csvserver&) = delete;

		actor_csvserver();
	public:
		friend class actor_instance<actor_csvserver>;
		static actor_csvserver& getInstance()
		{
			return actor_instance<actor_csvserver>::instance();
		}

		virtual ~actor_csvserver() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);

		bool handle(const message<np_actor_csv_verify_version>& adata);
	};
}//namespace ngl