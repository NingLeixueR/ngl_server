#include "actor_log.h"

namespace ngl
{
	actor_log::actor_log(i32_actordataid aid):
		m_log(nullptr),
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_LOG, 
					.m_area = ttab_servers::tab()->m_area,
					.m_id = aid
				},
				.m_weight = 0x7fffffff
			})
	{
		ENUM_ACTOR lactortype = nlogactor::actor_type(aid);
		const char* lname = (lactortype == ACTOR_NONE) ? "sys_global" : nactortype::enum2name(lactortype);

		logfile::config lconfig
		{
			.m_type = nlogactor::log_type(aid),
			.m_dir = "log/" + ttab_servers::tab()->m_name + "/"+ lname,
			.m_flush_time = 10,
		};
		nconfig::get_publicconfig()->find("log_flushtime", lconfig.m_flush_time);
		m_log = logfile::create_make(nlogactor::log_type(aid) != ELOG_LOCAL, lconfig);
	}

	void actor_log::init()
	{}

	i64_actorid actor_log::actorid(ENUM_ACTOR aactortype, ELOG_TYPE alogtype)
	{
		nlogactor ltemp(aactortype, alogtype);
		return nguid::make(ACTOR_LOG, ttab_servers::tab()->m_area, ltemp.m_value32);
	}

	void actor_log::nregister()
	{
		register_handle_custom<actor_log>::func<
			np_actor_logitem
		>(false);
	}

	actor_log::~actor_log()
	{
	}

	bool actor_log::handle(message<np_actor_logitem>& adata)
	{
		logitem& ldata = adata.get_data()->m_data;
		m_log->printf(&ldata);
		return true;
	}
}//namespace ngl