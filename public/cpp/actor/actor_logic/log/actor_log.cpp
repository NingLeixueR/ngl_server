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
					.m_area = tab_self_area,
					.m_id = aid
				},
				.m_weight = 0x7fffffff
			})
	{
		logfile::config lconfig
		{
			.m_id = aid,
			.m_dir = std::format("log/{}", ttab_servers::tab()->m_name),
			.m_flush_time = 10,
		};
		nconfig::get_publicconfig()->find("log_flushtime", lconfig.m_flush_time);
		m_log = logfile::create_make(lconfig);
	}

	void actor_log::init()
	{}

	void actor_log::nregister()
	{
		register_handle_custom<actor_log>::func<
			np_logitem,
			np_logflush
		>(false);
	}

	actor_log::~actor_log()
	{
	}

	bool actor_log::handle(const message<np_logitem>& adata)
	{
		const np_logitem* ldata = adata.get_data();
		m_log->printf(ldata);
		return true;
	}

	bool actor_log::handle(const message<np_logflush>& adata)
	{
		m_log->flush();
		return true;
	}
}//namespace ngl