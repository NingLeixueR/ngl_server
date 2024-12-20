#pragma once

#include "actor_manage.h"
#include "actor_role.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_manage_role : public actor
	{
		actor_manage_role(const actor_manage_role&) = delete;
		actor_manage_role& operator=(const actor_manage_role&) = delete;

		actor_manage_role();

		std::set<int64_t> m_roleban;
		using nclient_keyvalue = tdb_keyvalue::nsp_cli<actor_manage_role>;
	public:
		friend class actor_instance<actor_manage_role>;
		static actor_manage_role& getInstance() 
		{ 
			return actor_instance<actor_manage_role>::instance(); 
		}

		virtual ~actor_manage_role() {}

		static void nregister();

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_MANAGE_ROLE;
		}

		static i64_actorid actorid()
		{
			return nguid::make(actor_type(), nguid::none_area(), ttab_servers::tab()->m_tcount);
		}
			
		bool handle(const message<np_roleban>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);

		//# gm«Î«Û
		using handle_gm = cmd<actor_role, std::string, int, const ngl::json_read&>;
		bool handle(const message<mforward<np_gm>>& adata);
	};
}//namespace ngl

