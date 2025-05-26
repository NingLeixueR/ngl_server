#include "actor_brief.h"
#include "actor_role.h"
#include "roleinfo.h"

namespace ngl
{
	void roleinfo::sync_actor_brief()
	{
		auto pro = std::make_shared<np_channel_data<pbdb::db_brief>>();
		pro->m_data.make();
		pro->m_data.m_data->insert(std::make_pair(actor()->id_guid(), get_constbrief()));
	
		// ²âÊÔ´úÂë
		tools::print_json2proto(get_constbrief());

		actor::static_send_actor(actor_brief::actorid(), nguid::make(), pro);
	}

	void roleinfo::initdata()
	{
		log_error()->print("roleinfo load finish {}", data());
	}

	const i64_actorid roleinfo::m_id()
	{
		return (int)db()->getconst().m_id();
	}

	data_modified<pbdb::db_role>& roleinfo::get_role()
	{
		return data()[actor()->id_guid()];
	}

	pbdb::db_brief& roleinfo::get_brief()
	{
		return *get_role().get().mutable_m_base();
	}

	const pbdb::db_brief& roleinfo::get_constbrief()
	{
		return get_role().getconst().m_base();
	}

	void roleinfo::change_event(eevents_logic type, int abegvalue, int32_t anowvalue)
	{
		np_eevents_logic_rolevaluechange lparm(type);
		lparm.m_actorid = actor()->id_guid();
		lparm.m_beforevalue = abegvalue;
		lparm.m_nowvalue = anowvalue;
		actor_events_logic::trigger_event(lparm);
	}

	int32_t roleinfo::lv()
	{
		return get_constbrief().m_lv();
	}

	void roleinfo::change_lv(int avalues)
	{
		pbdb::db_brief& lrb = get_brief();
		change_event(eevents_logic::eevents_logic_rolelevelchange, lrb.m_lv(), lrb.m_lv() + avalues);
		lrb.set_m_lv(lrb.m_lv() + avalues);
		sync_actor_brief();
		static_task::update_change(actor(), ETaskRoleLv, lrb.m_lv());
	}

	int32_t roleinfo::vip()
	{
		return get_constbrief().m_vip();
	}

	void roleinfo::change_vip(int avalues)
	{
		pbdb::db_brief& lrb = get_brief();
		lrb.set_m_vip(lrb.m_vip() + avalues);
		sync_actor_brief();
		static_task::update_change(actor(), ETaskRoleLv, lrb.m_lv());
	}

	const char* roleinfo::name()
	{
		return get_constbrief().m_name().c_str();
	}

	void roleinfo::rename(const char* aname)
	{
		pbdb::db_brief& lrb = get_brief();
		lrb.set_m_name(aname);
		sync_actor_brief();
	}

	int32_t roleinfo::gold()
	{
		return get_constbrief().m_moneygold();
	}

	void roleinfo::change_gold(int avalues)
	{
		pbdb::db_brief& lrb = get_brief();
		change_event(eevents_logic::eevents_logic_rolelevelchange, lrb.m_moneygold(), lrb.m_moneygold() + avalues);
		lrb.set_m_moneygold(lrb.m_moneygold() + avalues);
		sync_actor_brief();
	}

	int32_t roleinfo::silver()
	{
		return get_constbrief().m_moneysilver();
	}

	void roleinfo::change_silver(int avalues)
	{
		pbdb::db_brief& lrb = get_brief();
		lrb.set_m_moneysilver(lrb.m_moneysilver() + avalues);
		sync_actor_brief();
	}

	int32_t roleinfo::notalkutc()
	{
		return get_constbrief().m_notalkutc();
	}

	void roleinfo::change_notalkutc(int avalues)
	{
		pbdb::db_brief& lrb = get_brief();
		lrb.set_m_notalkutc(avalues);
		sync_actor_brief();
	}

	bool roleinfo::bantalk()
	{
		int32_t lnow = (int32_t)localtime::gettime();
		return lnow < get_constbrief().m_notalkutc();
	}

}// namespace ngl