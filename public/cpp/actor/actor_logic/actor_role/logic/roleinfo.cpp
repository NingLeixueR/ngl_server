#include "actor_brief.h"
#include "actor_role.h"
#include "roleinfo.h"

namespace ngl
{
	void roleinfo::initdata()
	{
		log_error()->print("roleinfo load finish {}", data());
	}

	const i64_actorid roleinfo::roleid()
	{
		return (int)db()->getconst()->mid();
	}

	pbdb::db_brief* roleinfo::get_brief()
	{
		static tdb_brief::nsp_cwrite<actor_role>* lpdb = nsp_instance<tdb_brief::nsp_cwrite<actor_role>>::nclient(get_actor()->id_guid());
		if (lpdb == nullptr)
		{
			tools::no_core_dump();
			return nullptr;
		}
		return lpdb->get(get_actor()->id_guid());
	}

	void roleinfo::change()
	{
		static tdb_brief::nsp_cwrite<actor_role>* lpdb = nsp_instance<tdb_brief::nsp_cwrite<actor_role>>::nclient(get_actor()->id_guid());
		if (lpdb == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		lpdb->change();
	}

	const pbdb::db_brief* roleinfo::get_constbrief()
	{
		static tdb_brief::nsp_cwrite<actor_role>* lpdb = nsp_instance<tdb_brief::nsp_cwrite<actor_role>>::nclient(get_actor()->id_guid());
		if (lpdb == nullptr)
		{
			tools::no_core_dump();
			return nullptr;
		}
		return lpdb->getconst(get_actor()->id_guid());
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
			log_error()->print(
				"roleinfo::lv fail role[{}]"
				, nguid(get_actor()->id_guid())
			);
			return -1;
		}
		return lrb->m_base().mlv();
	}

	void roleinfo::change_lv(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::change_lv fail role[{}] value[{}]"
				, nguid(get_actor()->id_guid())
				, avalues
			);
			return;
		}
		int32_t loldvalue = lrb->m_base().mlv();
		lrb->mutable_m_base()->set_mlv(loldvalue + avalues);
		change();
		static_task::update_change(nactor(), ETaskRoleLv, lrb->m_base().mlv());
		change_event(eevents_logic::eevents_logic_rolelevelchange, loldvalue, loldvalue + avalues);
	}

	int32_t roleinfo::vip()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::vip fail role[{}]"
				, nguid(get_actor()->id_guid())
			);
			return -1;
		}
		return lrb->m_base().mvip();
	}

	void roleinfo::change_vip(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::change_vip fail role[{}] value[{}]"
				, nguid(get_actor()->id_guid())
				, avalues
			);
			return;
		}
		lrb->mutable_m_base()->set_mvip(lrb->m_base().mvip() + avalues);
		change();
		static_task::update_change(nactor(), ETaskRoleLv, lrb->m_base().mlv());
	}

	const char* roleinfo::name()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::name fail role[{}]"
				, nguid(get_actor()->id_guid())
			);
			return "";
		}
		return lrb->m_base().mname().c_str();
	}

	void roleinfo::rename(const char* aname)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::rename fail role[{}] value[{}]"
				, nguid(get_actor()->id_guid())
				, aname
			);
			return;
		}
		lrb->mutable_m_base()->set_mname(aname);
		change();
	}

	int32_t roleinfo::gold()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::gold fail role[{}]"
				, nguid(get_actor()->id_guid())
			);
			return -1;
		}
		return lrb->m_base().mmoneygold();
	}

	void roleinfo::change_gold(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::change_gold fail role[{}] value[{}]"
				, nguid(get_actor()->id_guid())
				, avalues
			);
			return;
		}
		int32_t loldvalue = lrb->m_base().mmoneygold();
		lrb->mutable_m_base()->set_mmoneygold(loldvalue + avalues);
		change();

		change_event(eevents_logic::eevents_logic_rolegoldchange, loldvalue, loldvalue + avalues);
		log_bi()->print("add_gold|{}|{}|{}|{}", lrb->mid(), lrb->m_base().mname(), loldvalue, lrb->m_base().mmoneygold());
	}

	int32_t roleinfo::silver()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::silver fail role[{}]"
				, nguid(get_actor()->id_guid())
			);
			return -1;
		}
		return lrb->m_base().mmoneysilver();
	}

	void roleinfo::change_silver(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::change_silver fail role[{}] value[{}]"
				, nguid(get_actor()->id_guid())
				, avalues
			);
			return;
		}
		int32_t loldvalue = lrb->m_base().mmoneysilver();
		lrb->mutable_m_base()->set_mmoneysilver(loldvalue + avalues);
		change();
		log_bi()->print("add_silver|{}|{}|{}|{}", lrb->mid(), lrb->m_base().mname(), loldvalue, lrb->m_base().mmoneysilver());
	}

	int32_t roleinfo::notalkutc()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::notalkutc fail role[{}]"
				, nguid(get_actor()->id_guid())
			);
			return -1;
		}
		return lrb->m_base().mnotalkutc();
	}

	void roleinfo::change_notalkutc(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::change_notalkutc fail role[{}] value[{}]"
				, nguid(get_actor()->id_guid())
				, avalues
			);
			return;
		}
		lrb->mutable_m_base()->set_mnotalkutc(avalues);
		change();
	}

	bool roleinfo::bantalk()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print(
				"roleinfo::bantalk fail role[{}]"
				, nguid(get_actor()->id_guid())
			);
			return false;
		}
		int32_t lnow = (int32_t)localtime::gettime();
		return lnow < lrb->m_base().mnotalkutc();
	}

}// namespace ngl