#include "actor_log.h"


namespace ngl
{
	actor_log::actor_log(ELOG_TYPE atype) :
		m_log(nullptr),
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_LOG, 
					.m_area = ttab_servers::tab()->m_area,
					.m_id = atype
				},
				.m_weight = 0x7fffffff
			})
	{
		logfile::config lconfig
		{
			.m_type = atype,
			.m_dir = "log/" + ttab_servers::tab()->m_name,
			.m_flush_time = 60,
		};
		m_log = logfile::create_make(atype != ELOG_LOCAL, lconfig);
	}

	void actor_log::init()
	{}

	void actor_log::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_log>(false
			, null<actor_logitempro>
		);
	}

	actor_log::~actor_log()
	{
	}

	bool actor_log::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_logitempro& adata)
	{
		actor_logitem& ldata = adata.m_data;
		m_log->printf(ldata);
		return true;
	}
}