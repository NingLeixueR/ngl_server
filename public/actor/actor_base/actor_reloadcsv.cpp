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
		make_timerparm::make_interval(tparm, 60);
		set_timer(tparm);
	}

	void actor_reloadcsv::actor_register()
	{
		actor_reloadcsv::register_timer<actor_reloadcsv>(&actor_reloadcsv::timer_handle);
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_reloadcsv>(
			false,
			null<actor_reloadcsv_pro>
		);
	}

	actor_reloadcsv::~actor_reloadcsv()
	{
	}

	bool actor_reloadcsv::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_reloadcsv_pro& adata)
	{
		for (auto& [key, value] : adata.m_csvcontent)
		{
			reload_csv::save(key, value);
		}
		actor_suspendthread ltemp;
		for (auto& [key, value] : adata.m_csvcontent)
		{
			reload_csv::reload(key);
		}
		return true;
	}

	bool actor_reloadcsv::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		LogLocalError("############actor_reloadcsv::timer_handle###########");
		actor_reloadcsv_verify_version pro;
		allcsv::foreach_version(pro.m_version);
		i64_actorid lrequest = actor_guid::make(ACTOR_RELOADCSV_DISTRIBUTE, tab_self_area, ttab_servers::tab()->m_reloadcsv);
		sendtoserver(ttab_servers::tab()->m_reloadcsv, pro, lrequest, id_guid());
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

	void actor_reloadcsv_distribute::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_reloadcsv_distribute>(
			false,
			null<actor_reloadcsv_verify_version>
		);
	}

	actor_reloadcsv_distribute::~actor_reloadcsv_distribute()
	{
	}

	bool actor_reloadcsv_distribute::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_reloadcsv_verify_version& adata)
	{
		LogLocalError("############actor_reloadcsv_distribute::handle###########");
		actor_reloadcsv_pro pro;
		std::map<std::string, csvbase*>& lversion = allcsv::all();
		for (auto [key, value] : lversion)
		{
			auto itor = adata.m_version.find(key);
			if (itor != adata.m_version.end() && itor->second >= value->version())
				continue;
			reload_csv::readcsv(key, pro.m_csvcontent[key]);
		}
		if (pro.m_csvcontent.empty() == false)
			send(apack->m_id, pro, apack->m_head.get_request_actor(), id_guid());
		return true;
	}

}