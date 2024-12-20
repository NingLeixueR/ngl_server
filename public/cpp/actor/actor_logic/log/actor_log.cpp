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

	void actor_log::nregister()
	{
		// ¶¨Ê±Æ÷
		actor::register_timer<actor_log>(&actor_log::timer_handle);

		register_handle_custom<actor_log>::func<
			np_logitem
		>(false);
	}

	actor_log::~actor_log()
	{
	}

	void actor_log::init()
	{
		// flush
		int32_t lflushtime = m_log->m_config.flush_time()/1000;
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, lflushtime) == false)
		{
			log_error()->print("actor_log::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		auto lparmflush = std::make_shared<log_timerparm>(log_timerparm::e_logflush);
		tparm.set_parm(lparmflush);
		set_timer(tparm);

		// create
		timerparm tparmcreate;
		if (make_timerparm::day(tparmcreate, 0, 0, 0) == false)
		{
			log_error()->print("actor_log::init() make_timerparm::make_timerparm::day(tparmcreate, 0, 0, 0) == false!!!");
			return;
		}
		auto lparmcreate = std::make_shared<log_timerparm>(log_timerparm::e_create);
		tparmcreate.set_parm(lparmcreate);
		set_timer(tparmcreate);
	}

	bool actor_log::handle(const message<np_logitem>& adata)
	{
		const np_logitem* ldata = adata.get_data();
		m_log->printf(ldata);
		return true;
	}

	bool actor_log::timer_handle(const message<timerparm>& adata)
	{
		std::shared_ptr<log_timerparm> lparm = std::static_pointer_cast<log_timerparm>(adata.get_data()->m_parm);
		if (lparm->m_type == log_timerparm::e_logflush)
		{
			m_log->flush();
			return true;
		}
		if (lparm->m_type == log_timerparm::e_create)
		{
			m_log->close_fstream();
			m_log->create();
			return true;
		}
		return true;
	}
}//namespace ngl