#pragma once

#include "actor.h"

namespace ngl
{
	class actor_csvserver
		: public actor
	{
		actor_csvserver();
	public:
		friend class actor_instance<actor_csvserver>;
		static actor_csvserver& getInstance()
		{
			return actor_instance<actor_csvserver>::instance();
		}

		virtual void init();

		static void nregister();

		virtual ~actor_csvserver();

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_CSVSERVER, nguid::none_area(), nguid::none_actordataid());
		}

		bool handle(message<np_actor_csv_verify_version>& adata);
	};
}//namespace ngl