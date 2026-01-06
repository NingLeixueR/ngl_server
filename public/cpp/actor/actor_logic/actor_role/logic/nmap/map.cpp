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
#include "events_map.h"
#include "ttab_map.h"
#include "map.h"

namespace ngl
{
	aoimap::aoimap() :
		m_tabid(-1)
	{}

	unit_role* aoimap::find_role(i64_actorid aid)
	{
		unit_role** lp = tools::findmap(m_roleunit, aid);
		return lp == nullptr ? nullptr : *lp;
	}

	unit_monster* aoimap::find_monster(i64_actorid aid)
	{
		unit_monster** lp = tools::findmap(m_monster, aid);
		return lp == nullptr ? nullptr : *lp;
	}

	unit* aoimap::find_unit(i64_actorid aid)
	{
		unit* lpunit = find_role(aid);
		if (lpunit != nullptr)
		{
			return lpunit;
		}
		lpunit = find_monster(aid);
		if (lpunit != nullptr)
		{
			return lpunit;
		}
		return nullptr;
	}

	bool aoimap::init(int32_t atid)
	{
		m_tabid = atid;
		auto tab = ttab_map::instance().tab(atid);
		if (tab == nullptr)
		{
			log_error()->print("allcsv::tab<tab_map>({}) == nullptr", m_tabid);
			return false;
		}
		m_grids.init(tab->m_w, tab->m_l, tab->m_nx, tab->m_ny);
		m_obstacles.init(tab->m_w, tab->m_l, tab->m_obstaclenx, tab->m_obstacleny);
		m_obstacles.set_obstacles(tab->m_obstacles);
		return true;
	}

	bool aoimap::copy_unit(unit* aunit, pbdb::UNIT* aUNIT)
	{
		if (aunit == nullptr)
		{
			return false;
		}
		aUNIT->set_mid(aunit->id());
		aUNIT->set_mtype(aunit->type());
		*aUNIT->mutable_mposition() = aunit->position();
		return true;
	}

	bool aoimap::copy_unit(i64_actorid aid, pbdb::UNIT* aunit)
	{
		unit* lpunit = find_unit(aid);
		return copy_unit(lpunit, aunit);
	}

	bool aoimap::enter(unit* aunit, int32_t ax, int32_t ay)
	{
		int32_t lenter_pos = m_grids.id(ax, ay);

		if (find_unit(aunit->id()) != nullptr)
		{
			log_error()->print("aoi_map::enter(tid={},id={}) enter find_unit() != nullptr", m_tabid, aunit->id());
			return false;
		}

		std::set<int32_t> lgrids1;
		std::set<int32_t> lgrids2;
		if (aunit->is_position_invalid() == false)
		{// 根据位置获取周围格子
			m_grids.idaround_list(m_grids.id(aunit->x(), aunit->y()), lgrids1);
		}
		m_grids.idaround_list(lenter_pos, lgrids2);

		pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW pro;
		pro.add_munits(aunit->id());
		std::set<i64_actorid> lview;
		// leave 离开视野
		for (int32_t id : lgrids1)
		{
			if (lgrids2.find(id) == lgrids2.end())
			{
				grid* lpgrid = m_grids.get_grid(id);
				if (lpgrid != nullptr)
				{
					const std::set<i64_actorid> lpset = lpgrid->get_unitlist();
					for (i64_actorid item : lpset)
					{
						lview.insert(item);
					}
				}
			}
		}
		if (lview.empty() == false)
		{
			pro.set_misenter(false);
			actor::send_client(lview, pro);

			np_eevents_map_leaveview lparm;
			lparm.m_leaveunitid = aunit->id();
			lparm.m_unitids.swap(lview);
			actor_events_map::trigger_event(lparm);
		}

		lview.clear();
		// enter 进入视野
		for (int32_t id : lgrids2)
		{
			if (lgrids1.contains(id) == false)
			{
				grid* lpgrid = m_grids.get_grid(id);
				if (lpgrid != nullptr)
				{
					const std::set<i64_actorid>& lpset = lpgrid->get_unitlist();
					for (i64_actorid item : lpset)
					{
						lview.insert(item);
					}
				}
			}
		}
		if (lview.empty() == false)
		{
			pro.set_misenter(true);
			actor::send_client(lview, pro);

			np_eevents_map_enterview lparm;
			lparm.m_enterunitid = aunit->id();
			lparm.m_unitids.swap(lview);
			actor_events_map::trigger_event(lparm);
		}

		aunit->set_x(ax);
		aunit->set_y(ay);
		return true;
	}

	void aoimap::leave(unit* aunit)
	{
		int lgrid = m_grids.id(aunit->x(), aunit->y());
		m_grids.leave(aunit->id(), lgrid);

		pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW pro;
		pro.add_munits(aunit->id());
		pro.set_misenter(false);

		std::set<int32_t> lgrids;
		m_grids.idaround_list(m_grids.id(aunit->x(), aunit->y()), lgrids);
		for (int32_t id : lgrids)
		{
			grid* lpgrid = m_grids.get_grid(id);
			if (lpgrid != nullptr)
			{
				const std::set<i64_actorid>& lpset = lpgrid->get_unitlist();
				actor::send_client(lpset, pro);
			}
		}
	}

	void aoimap::sync_position(unit* aunit, int32_t agridid)
	{
		pbnet::PROBUFF_NET_SYNC_POSITION pro;
		(*pro.mutable_mposition())[aunit->id()] = aunit->position();

		std::set<int32_t> lgrids;
		m_grids.idaround_list(agridid, lgrids);
		for (int32_t id : lgrids)
		{
			grid* lpgrid = m_grids.get_grid(id);
			if (lpgrid != nullptr)
			{
				const std::set<i64_actorid>& lpset = lpgrid->get_unitlist();
				actor::send_client(lpset, pro);
			}
		}
	}

	bool aoimap::move(unit* aunit, int32_t ax, int32_t ay)
	{
		int32_t llast_pos = m_grids.id(aunit->x(), aunit->y());
		int32_t lenter_pos = m_grids.id(ax, ay);

		if (llast_pos == lenter_pos)
		{
			aunit->set_x(ax);
			aunit->set_y(ay);
			sync_position(aunit, lenter_pos);
		}
		else
		{
			enter(aunit, ax, ay);
			sync_position(aunit, llast_pos);
			sync_position(aunit, lenter_pos);
		}
		return true;
	}

	// 改变方向
	void aoimap::change_angle(i64_actorid aunitid, int32_t aangle)
	{
		unit* lpunit = find_unit(aunitid);
		if (lpunit == nullptr)
		{
			return;
		}
		lpunit->set_angle(aangle);
	}

	// 改变速度
	void aoimap::change_speed(i64_actorid aunitid, int32_t aspeed)
	{
		unit* lpunit = find_unit(aunitid);
		if (lpunit == nullptr)
		{
			return;
		}
		lpunit->set_speed(aspeed);
	}

	void aoimap::change(i64_actorid aunitid, pbdb::POSITION& aposition)
	{
		change_angle(aunitid, aposition.mangle());
		change_speed(aunitid, aposition.mspeed());
	}

	void aoimap::update(int64_t ams)
	{
		pbdb::VECTOR2 lpos;
		for (std::pair<const i64_actorid, unit_role*>& item : m_roleunit)
		{
			move(item.second, lpos.mx(), lpos.my());
			item.second->update(ams);
		}
		for (std::pair<const i64_actorid, unit_monster*>& item : m_monster)
		{
			move(item.second, lpos.mx(), lpos.my());
			item.second->update(ams);
		}
	}
}// namespace ngl