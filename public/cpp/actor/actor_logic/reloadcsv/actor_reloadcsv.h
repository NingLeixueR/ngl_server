#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "nprotocol.h"
#include "ndbclient.h"
#include "ntimer.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "db.h"
#include "net.h"

namespace ngl
{
	class actor_reloadcsv : public actor
	{
		actor_reloadcsv();
	public:
		friend class actor_instance<actor_reloadcsv>;
		static actor_reloadcsv& getInstance()
		{
			return actor_instance<actor_reloadcsv>::instance();
		}

		virtual void init();
		static void nregister();

		virtual ~actor_reloadcsv();

		bool handle(message<np_actor_reloadcsv>& adata);

		// 定时器
		// 间接定时器 
		bool timer_handle(message<timerparm>& adata);
	};

	class actor_reloadcsv_distribute
		: public actor
	{
		actor_reloadcsv_distribute();
	public:
		friend class actor_instance<actor_reloadcsv_distribute>;
		static actor_reloadcsv_distribute& getInstance()
		{
			return actor_instance<actor_reloadcsv_distribute>::instance();
		}

		virtual void init();
		static void nregister();

		virtual ~actor_reloadcsv_distribute();
				
		bool handle(message<np_actor_csv_verify_version>& adata);
	};
}//namespace ngl