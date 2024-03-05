#include "roleinfo.h"
#include "actor_role.h"


namespace ngl
{
	void roleinfo::sync_actor_roleinfo()
	{
		auto pro = std::make_shared<actor_roleinfo>();
		pro->m_vecinfo.make();
		pro->m_vecinfo.m_data->resize(1);
		//pbdb::db_brief& linfo = (*pro->m_vecinfo.m_data)[0];
		(*pro->m_vecinfo.m_data)[0] = m_base();

		std::string ljson;
		if (tools::protostr((*pro->m_vecinfo.m_data)[0], ljson))
		{
			LogLocalError("roleinfo::sync_actor_roleinfo [%]", ljson)
		}

		actor::static_send_actor(actor_guid::make_self(ACTOR_BRIEF), actor_guid::make(), pro);
	}

	void roleinfo::initdata()
	{
		LogLocalError("#[roleinfo]	#[load finish]")
		LogLocalError("#[id]		#[%]", actorbase()->id_guid())
		LogLocalError("#[name]		#[%]", name())
		LogLocalError("#[lv]		#[%]", lv())
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
		actor()->m_task.task_condition(ETaskRoleLv, { avalues });
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
}