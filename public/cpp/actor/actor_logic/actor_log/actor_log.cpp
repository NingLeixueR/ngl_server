#include "actor_log.h"

namespace ngl
{
	actor_log::actor_log(int32_t atype):
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_LOG, 
					.m_area = tab_self_area,
					.m_id = nconfig::m_nodeid
				},
				.m_weight = 0x7fffffff
			})
	{
		for (int i = ELOG_DEFAULT;i< ELOG_COUNT;i = i<<1)
		{
			if ((atype & i) != 0)
			{
				logfile::config lconfig
				{
					.m_id = nconfig::m_nodeid,
					.m_dir = "log",
					.m_type = (ELOG_TYPE)i,
					.m_flush_time = 10,
				};
				nconfig::get_publicconfig()->find("log_flushtime", lconfig.m_flush_time);
				m_logarr[(ELOG_TYPE)i] = logfile::create_make(lconfig);
			}
		}
	}

	ENUM_ACTOR actor_log::actor_type()
	{
		return ACTOR_LOG;
	}

	i64_actorid actor_log::actorid(i32_actordataid aid)
	{
		return nguid::make(actor_type(), tab_self_area, aid);
	}

	void actor_log::init()
	{
		// ��DB�ṹ:DB.set(this);

		// ����timer_handle��ʱ��
		// flush
		
		int32_t lflushtime = m_logarr.begin()->second->m_config.flush_time() / 1000;
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, lflushtime) == false)
		{
			log_error()->print("actor_log::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		auto lparmflush = std::make_shared<log_timerparm>(log_timerparm::e_logflush);
		timerparm_set_parm(&tparm, lparmflush);
		set_timer(tparm);

		// create
		np_timerparm tparmcreate;
		if (make_timerparm::parm_day(tparmcreate, 0, 0, 0) == false)
		{
			log_error()->print("actor_log::init() make_timerparm::make_timerparm::day(tparmcreate, 0, 0, 0) == false!!!");
			return;
		}
		auto lparmcreate = std::make_shared<log_timerparm>(log_timerparm::e_create);
		timerparm_set_parm(&tparmcreate, lparmcreate);
		set_timer(tparmcreate);
	}

	void actor_log::loaddb_finish(bool adbishave)
	{
	}

	void actor_log::nregister()
	{
		// ��ʱ��
		actor::register_timer<actor_log>(&actor_log::timer_handle);

		// ���Զ���np_��Ϣ
		register_handle<actor_log>::func<
			np_logitem
		>(false);
	}
	
	bool actor_log::timer_handle(const message<np_timerparm>& adata)
	{
		std::shared_ptr<log_timerparm> lparm = std::static_pointer_cast<log_timerparm>(adata.get_data()->m_parm);
		if (lparm->m_type == log_timerparm::e_logflush)
		{
			for (std::pair<const ELOG_TYPE, std::shared_ptr<logfile>>& item : m_logarr)
			{
				item.second->flush();
			}
			return true;
		}
		if (lparm->m_type == log_timerparm::e_create)
		{
			for (std::pair<const ELOG_TYPE, std::shared_ptr<logfile>>& item : m_logarr)
			{
				item.second->close_fstream();
				item.second->create();
			}
			return true;
		}
		return true;
	}

	bool actor_log::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl