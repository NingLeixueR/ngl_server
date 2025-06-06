#include "actor_brief.h"
#include "actor_role.h"
#include "roleinfo.h"

namespace ngl
{
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
		return data()[get_actor()->id_guid()];
	}

	pbdb::db_brief* roleinfo::get_brief()
	{
		return tdb_brief::nsp_cli<actor_role>::getInstance(get_actor()->id_guid()).get(get_actor()->id_guid());
	}

	const pbdb::db_brief* roleinfo::get_constbrief()
	{
		return tdb_brief::nsp_cli<actor_role>::getInstance(get_actor()->id_guid()).getconst(get_actor()->id_guid());
	}

	void roleinfo::change_event(eevents_logic type, int abegvalue, int32_t anowvalue)
	{
		np_eevents_logic_rolevaluechange lparm(type);
		lparm.m_actorid = get_actor()->id_guid();
		lparm.m_beforevalue = abegvalue;
		lparm.m_nowvalue = anowvalue;
		actor_events_logic::trigger_event(lparm);
	}

	int32_t roleinfo::lv()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::lv fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->m_lv();
	}

	void roleinfo::change_lv(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_lv fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		int32_t loldvalue = lrb->m_moneygold();
		lrb->set_m_lv(loldvalue + avalues);
		tdb_brief::nsp_cli<actor_role>::getInstance(get_actor()->id_guid()).change(get_actor()->id_guid());
		static_task::update_change(nactor(), ETaskRoleLv, lrb->m_lv());
		change_event(eevents_logic::eevents_logic_rolelevelchange, loldvalue, loldvalue + avalues);
	}

	int32_t roleinfo::vip()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::vip fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->m_vip();
	}

	void roleinfo::change_vip(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_vip fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		lrb->set_m_vip(lrb->m_vip() + avalues);
		tdb_brief::nsp_cli<actor_role>::getInstance(get_actor()->id_guid()).change(get_actor()->id_guid());
		static_task::update_change(nactor(), ETaskRoleLv, lrb->m_lv());
	}

	const char* roleinfo::name()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::name fail role[{}]", nguid(get_actor()->id_guid()));
			return "";
		}
		return lrb->m_name().c_str();
	}

	void roleinfo::rename(const char* aname)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::rename fail role[{}] value[{}]", nguid(get_actor()->id_guid()), aname);
			return;
		}
		lrb->set_m_name(aname);
		tdb_brief::nsp_cli<actor_role>::getInstance(get_actor()->id_guid()).change(get_actor()->id_guid());
	}

	int32_t roleinfo::gold()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::gold fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->m_moneygold();
	}

	void roleinfo::change_gold(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_gold fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		int32_t loldvalue = lrb->m_moneygold();
		lrb->set_m_moneygold(loldvalue + avalues);
		tdb_brief::nsp_cli<actor_role>::getInstance(get_actor()->id_guid()).change(get_actor()->id_guid());

		change_event(eevents_logic::eevents_logic_rolegoldchange, loldvalue, loldvalue + avalues);
	}

	int32_t roleinfo::silver()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::silver fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->m_moneysilver();
	}

	void roleinfo::change_silver(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_silver fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		lrb->set_m_moneysilver(lrb->m_moneysilver() + avalues);
		tdb_brief::nsp_cli<actor_role>::getInstance(get_actor()->id_guid()).change(get_actor()->id_guid());
	}

	int32_t roleinfo::notalkutc()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::notalkutc fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->m_notalkutc();
	}

	void roleinfo::change_notalkutc(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_notalkutc fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		lrb->set_m_notalkutc(avalues);
		tdb_brief::nsp_cli<actor_role>::getInstance(get_actor()->id_guid()).change(get_actor()->id_guid());
	}

	bool roleinfo::bantalk()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::bantalk fail role[{}]", nguid(get_actor()->id_guid()));
			return false;
		}
		int32_t lnow = (int32_t)localtime::gettime();
		return lnow < lrb->m_notalkutc();
	}

}// namespace ngl