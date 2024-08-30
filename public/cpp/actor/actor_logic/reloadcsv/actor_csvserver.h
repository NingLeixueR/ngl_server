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

		virtual void init();

		static void nregister();

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_CSVSERVER;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_CSVSERVER, nguid::none_area(), nguid::none_actordataid());
		}


		virtual ~actor_csvserver();

		bool handle(message<np_actor_csv_verify_version>& adata);
	};
}//namespace ngl