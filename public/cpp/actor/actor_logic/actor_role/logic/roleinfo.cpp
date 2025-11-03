/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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
			log_error()->print("roleinfo::lv fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->mbase().mlv();
	}

	void roleinfo::change_lv(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_lv fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		int32_t loldvalue = lrb->mbase().mlv();
		lrb->mutable_mbase()->set_mlv(loldvalue + avalues);
		static_task::update_change(nactor(), ETaskRoleLv, lrb->mbase().mlv());
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
		return lrb->mbase().mvip();
	}

	void roleinfo::change_vip(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_vip fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		lrb->mutable_mbase()->set_mvip(lrb->mbase().mvip() + avalues);
		static_task::update_change(nactor(), ETaskRoleLv, lrb->mbase().mlv());
	}

	const char* roleinfo::name()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::name fail role[{}]", nguid(get_actor()->id_guid()));
			return "";
		}
		return lrb->mbase().mname().c_str();
	}

	void roleinfo::rename(const char* aname)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::rename fail role[{}] value[{}]", nguid(get_actor()->id_guid()), aname);
			return;
		}
		lrb->mutable_mbase()->set_mname(aname);
	}

	int32_t roleinfo::gold()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::gold fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->mbase().mmoneygold();
	}

	void roleinfo::change_gold(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_gold fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		int32_t loldvalue = lrb->mbase().mmoneygold();
		lrb->mutable_mbase()->set_mmoneygold(loldvalue + avalues);
		change_event(eevents_logic::eevents_logic_rolegoldchange, loldvalue, loldvalue + avalues);
		log_bi()->print("add_gold|{}|{}|{}|{}", lrb->mid(), lrb->mbase().mname(), loldvalue, lrb->mbase().mmoneygold());
	}

	int32_t roleinfo::silver()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::silver fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->mbase().mmoneysilver();
	}

	void roleinfo::change_silver(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_silver fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		int32_t loldvalue = lrb->mbase().mmoneysilver();
		lrb->mutable_mbase()->set_mmoneysilver(loldvalue + avalues);
		log_bi()->print("add_silver|{}|{}|{}|{}", lrb->mid(), lrb->mbase().mname(), loldvalue, lrb->mbase().mmoneysilver());
	}

	int32_t roleinfo::notalkutc()
	{
		const pbdb::db_brief* lrb = get_constbrief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::notalkutc fail role[{}]", nguid(get_actor()->id_guid()));
			return -1;
		}
		return lrb->mbase().mnotalkutc();
	}

	void roleinfo::change_notalkutc(int avalues)
	{
		pbdb::db_brief* lrb = get_brief();
		if (lrb == nullptr)
		{
			log_error()->print("roleinfo::change_notalkutc fail role[{}] value[{}]", nguid(get_actor()->id_guid()), avalues);
			return;
		}
		lrb->mutable_mbase()->set_mnotalkutc(avalues);
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
		return lnow < lrb->mbase().mnotalkutc();
	}

}// namespace ngl