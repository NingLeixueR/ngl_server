#pragma once

#include "actor_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_switchprocess.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
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
		static void actor_register();

		virtual ~actor_reloadcsv();

		enum { ACTOR_TYPE = ACTOR_RELOADCSV};

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_reloadcsv_pro& adata);

		// 定时器
		// 间接定时器 
		bool timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata);
	private:

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
		static void actor_register();

		virtual ~actor_reloadcsv_distribute();

		enum { ACTOR_TYPE = ACTOR_RELOADCSV_DISTRIBUTE};

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_reloadcsv_verify_version& adata);
	private:
	};

}