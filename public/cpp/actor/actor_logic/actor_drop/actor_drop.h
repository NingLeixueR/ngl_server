#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "net.pb.h"
#include "drop.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_drop : 
		public actor
	{
		actor_drop(const actor_drop&) = delete;
		actor_drop& operator=(const actor_drop&) = delete;

		drop<actor_drop> m_drop;

		actor_drop();
	public:
		friend class actor_instance<actor_drop>;
		static actor_drop& getInstance()
		{
			return actor_instance<actor_drop>::instance();
		}

		virtual ~actor_drop() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		static bool use(int aid, int acount, i64_actorid aroleid, const std::string& asrc = "", std::map<int, int>* amap = nullptr, int32_t amailid = -1)
		{
			return getInstance().m_drop.use(aid, acount, aroleid, asrc, amap, amailid);
		}

		static bool droplist(int aid, int acount, std::map<int, int>& amap)
		{
			return getInstance().m_drop.droplist(aid, acount, amap);
		}

		static bool droplist(int aid, int acount, google::protobuf::Map<int32_t, int32_t>& amap)
		{
			return getInstance().m_drop.droplist(aid, acount, amap);
		}

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
	};
}//namespace ngl
