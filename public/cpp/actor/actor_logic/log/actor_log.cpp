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

	void actor_log::nregister()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_log>(false
			, dregister_fun_handle(actor_log, np_actor_logitem)
		);
	}

	actor_log::~actor_log()
	{
	}

	bool actor_log::handle(message<np_actor_logitem>& adata)
	{
		logitem& ldata = adata.m_data->m_data;
		m_log->printf(ldata);
		return true;
	}
}//namespace ngl