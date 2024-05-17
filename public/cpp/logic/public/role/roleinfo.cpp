#include "actor_role.h"
#include "roleinfo.h"

namespace ngl
{
	void roleinfo::sync_actor_roleinfo()
	{
		auto pro = std::make_shared<np_actor_roleinfo>();
		pro->m_vecinfo.make();
		pro->m_vecinfo.m_data->resize(1);
		//pbdb::db_brief& linfo = (*pro->m_vecinfo.m_data)[0];
		(*pro->m_vecinfo.m_data)[0] = m_base();

		std::string ljson;
		if (tools::protostr((*pro->m_vecinfo.m_data)[0], ljson))
		{
			log_error()->print("roleinfo::sync_actor_roleinfo [{}]", ljson);
		}

		actor::static_send_actor(nguid::make_self(ACTOR_BRIEF), nguid::make(), pro);
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

	const pbdb::db_role& roleinfo::m_role()
	{
		return db()->getconst();
	}

	const pbdb::db_brief& roleinfo::m_base()
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
		sync_actor_roleinfo();
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
		sync_actor_roleinfo();
		static_task::update_change(actor(), ETaskRoleVip, lrb->m_vip());
	}

	const char* roleinfo::name()
	{
		pbdb::db_brief* lrb = db()->get(false).mutable_m_base();
		return lrb->m_name().c_str();
	}

	void roleinfo::change_name(const char* aname)
	{
		pbdb::db_brief* lrb = db()->get(true).mutable_m_base();
		lrb->set_m_name(aname);
		sync_actor_roleinfo();
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
		sync_actor_roleinfo();
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
		sync_actor_roleinfo();
	}
}// namespace ngl