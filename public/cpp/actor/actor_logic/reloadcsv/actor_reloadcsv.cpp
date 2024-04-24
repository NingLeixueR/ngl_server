#include "actor_reloadcsv.h"
#include "manage_csv.h"
#include "xmlnode.h"
#include "splite.h"

namespace ngl
{
	actor_reloadcsv::actor_reloadcsv() :
		actor(
			actorparm
			{
				.m_parm{.m_type = ACTOR_RELOADCSV, .m_id = nconfig::m_nodeid},
				.m_weight = 0x7fffffff,
			})
	{}

	void actor_reloadcsv::init()
	{
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, 60) == false)
		{
			LogLocalError("actor_reloadcsv::init() make_timerparm::make_interval(tparm, 60) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	void actor_reloadcsv::nregister()
	{
		actor_reloadcsv::register_timer<actor_reloadcsv>(&actor_reloadcsv::timer_handle);
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_reloadcsv>(
			false
			, dregister_fun_handle(actor_reloadcsv, np_actor_reloadcsv)
		);
	}

	actor_reloadcsv::~actor_reloadcsv()
	{
	}

	bool actor_reloadcsv::handle(message<np_actor_reloadcsv>& adata)
	{
		for (auto& [key, value] : adata.m_data->m_csvcontent)
		{
			reload_csv::save(key, value);
		}
		actor_suspendthread ltemp;
		for (auto& [key, value] : adata.m_data->m_csvcontent)
		{
			reload_csv::reload(key);
		}
		return true;
	}

	bool actor_reloadcsv::timer_handle(message<timerparm>& adata)
	{
		LogLocalError("############actor_reloadcsv::timer_handle###########")
		np_actor_csv_verify_version pro;
		allcsv::foreach_version(pro.m_version);
		i64_actorid lrequest = nguid::make(ACTOR_RELOADCSV_DISTRIBUTE, tab_self_area, ttab_servers::tab()->m_reloadcsv);
		send_server(ttab_servers::tab()->m_reloadcsv, pro, lrequest, id_guid());
		return true;
	}

	actor_reloadcsv_distribute::actor_reloadcsv_distribute() :
		actor(
			actorparm
			{
				.m_parm{.m_type = ACTOR_RELOADCSV_DISTRIBUTE, .m_id = nconfig::m_nodeid,},
				.m_weight = 0x7fffffff,
			})
	{}

	void actor_reloadcsv_distribute::init() {}

	void actor_reloadcsv_distribute::nregister()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_reloadcsv_distribute>(
			false
			, dregister_fun_handle(actor_reloadcsv_distribute, np_actor_csv_verify_version)
		);
	}

	actor_reloadcsv_distribute::~actor_reloadcsv_distribute()
	{
	}

	bool actor_reloadcsv_distribute::handle(message<np_actor_csv_verify_version>& adata)
	{
		LogLocalError("############actor_reloadcsv_distribute::handle###########")
		auto lparm = adata.m_data;
		auto lpack = adata.m_pack;
		np_actor_reloadcsv pro;
		std::map<std::string, csvbase*>& lversion = allcsv::all();
		for (auto [key, value] : lversion)
		{
			auto itor = lparm->m_version.find(key);
			if (itor != lparm->m_version.end() && itor->second >= value->version())
				continue;
			reload_csv::readcsv(key, pro.m_csvcontent[key]);
		}
		if (pro.m_csvcontent.empty() == false)
			send(lpack->m_id, pro, lpack->m_head.get_request_actor(), id_guid());
		return true;
	}
}//namespace ngl