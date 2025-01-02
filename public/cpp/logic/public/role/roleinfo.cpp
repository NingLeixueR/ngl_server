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
	
		// ≤‚ ‘¥˙¬Î
		tools::print_protojson(get_constbrief());

		actor::static_send_actor(actor_brief::actorid(), nguid::make(), pro);
	}

	void roleinfo::initdata()
	{
		auto lstream = log_error();
		(*lstream) << "#[roleinfo]	#[load finish]" << std::endl;
		(*lstream) << "#[id]		#["<< actorbase()->id_guid() <<"]" << std::endl;
		(*lstream) << "#[name]		#[" << name() << "]" << std::endl;
		(*lstream) << "#[lv]		#[" << lv() << "]" << std::endl;
		(*lstream).print("");
	}

	const int32_t roleinfo::m_id()
	{
		return db()->getconst().m_id();
	}

	const pbdb::db_role& roleinfo::get_constrole()
	{
		return db()->getconst();
	}

	const pbdb::db_brief& roleinfo::get_constbrief()
	{
		return db()->getconst().m_base();
	}

	int32_t roleinfo::lv()
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		return lrb->m_lv();
	}

	void roleinfo::change_lv(int avalues)
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		lrb->set_m_lv(lrb->m_lv() + avalues);
		sync_actor_brief();
		static_task::update_change(actor(), ETaskRoleLv, lrb->m_lv());
	}

	int32_t roleinfo::vip()
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		return lrb->m_vip();
	}

	void roleinfo::change_vip(int avalues)
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		lrb->set_m_vip(lrb->m_vip() + avalues);
		sync_actor_brief();
		static_task::update_change(actor(), ETaskRoleVip, lrb->m_vip());
	}

	const char* roleinfo::name()
	{
		pbdb::db_brief* lrb = db()->get(false).mutable_m_base();
		return lrb->m_name().c_str();
	}

	void roleinfo::rename(const char* aname)
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		lrb->set_m_name(aname);
		sync_actor_brief();
	}

	int32_t roleinfo::gold()
	{
		pbdb::db_brief* lrb = db()->get(false).mutable_m_base();
		return lrb->m_moneygold();
	}

	void roleinfo::change_gold(int avalues)
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		lrb->set_m_moneygold(lrb->m_moneygold() + avalues);
		sync_actor_brief();
	}

	int32_t roleinfo::silver()
	{
		pbdb::db_brief* lrb = db()->get(false).mutable_m_base();
		return lrb->m_moneysilver();
	}

	void roleinfo::change_silver(int avalues)
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		lrb->set_m_moneysilver(lrb->m_moneysilver() + avalues);
		sync_actor_brief();
	}

	int32_t roleinfo::notalkutc()
	{
		pbdb::db_brief* lrb = db()->get(false).mutable_m_base();
		return lrb->m_notalkutc();
	}

	void roleinfo::change_notalkutc(int avalues)
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		lrb->set_m_notalkutc(avalues);
		sync_actor_brief();
	}
}// namespace ngl