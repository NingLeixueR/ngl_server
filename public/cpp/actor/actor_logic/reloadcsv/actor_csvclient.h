#pragma once

#include "actor.h"

namespace ngl
{
	class actor_csvclient 
		: public actor
	{
		actor_csvclient(const actor_csvclient&) = delete;
		actor_csvclient& operator=(const actor_csvclient&) = delete;

		actor_csvclient();
	public:
		friend class actor_instance<actor_csvclient>;
		static actor_csvclient& getInstance()
		{
			return actor_instance<actor_csvclient>::instance();
		}

		virtual void init();

		static void nregister();

		virtual ~actor_csvclient();

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_CSVCLIENT;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_CSVCLIENT, tab_self_area, nconfig::m_nodeid);
		}

		bool handle(const message<np_actor_reloadcsv>& adata);

		// # ��ʱ��
		bool timer_handle(const message<timerparm>& adata);
	};
}//namespace ngl