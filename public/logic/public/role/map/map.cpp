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

	unit_region* aoimap::find_region(i64_actorid aid)
	{
		unit_region** lp = tools::findmap(m_region, aid);
		return lp == nullptr ? nullptr : *lp;
	}

	unit* aoimap::find_unit(i64_actorid aid)
	{
		unit* lpunit = find_role(aid);
		if (lpunit != nullptr)
			return lpunit;
		lpunit = find_monster(aid);
		if (lpunit != nullptr)
			return lpunit;
		lpunit = find_region(aid);
		if (lpunit != nullptr)
			return lpunit;
		return nullptr;
	}

	bool aoimap::init(int32_t atid)
	{
		m_tabid = atid;
		auto tab = allcsv::tab<tab_map>(atid);
		if (tab == nullptr)
		{
			LogLocalError("allcsv::tab<tab_map>(%) == nullptr", m_tabid);
			return false;
		}
		m_grids.init(tab->m_w, tab->m_l, tab->m_nx, tab->m_ny);
		m_obstacles.init(tab->m_w, tab->m_l, tab->m_obstaclenx, tab->m_obstacleny);
		m_obstacles.set_obstacles(tab->m_obstacles);
		return true;
	}

	bool aoimap::copy_unit(unit* aunit, pbnet::UNIT* aUNIT)
	{
		if (aunit == nullptr)
			return false;
		aUNIT->set_m_id(aunit->id());
		aUNIT->set_m_type(aunit->type());
		//pbnet::VECTOR2*;
		*aUNIT->mutable_m_position() = aunit->position();
		return true;
	}

	bool aoimap::get_unit(i64_actorid aid, pbnet::UNIT* aunit)
	{
		unit* lpunit = find_unit(aid);
		return copy_unit(lpunit, aunit);
	}

	void aoimap::set_enterview(const std::function<void(i64_actorid, std::set<i64_actorid>&)>& afun)
	{
		m_enterview = afun;
	}

	void aoimap::set_leaveview(const std::function<void(i64_actorid, std::set<i64_actorid>&)>& afun)
	{
		m_leaveview = afun;
	}

	bool aoimap::enter(unit* aunit, int32_t ax, int32_t ay)
	{
		int32_t lenter_pos = m_grids.id(ax, ay);

		if (find_unit(aunit->id()) != nullptr)
		{
			LogLocalError("aoi_map::enter(tid=%,id=%,agid=%) enter find_unit() != nullptr", m_tabid, aunit->id());
			return false;
		}

		std::set<int32_t> lgrids1;
		std::set<int32_t> lgrids2;
		if (aunit->is_position_invalid() == false)
		{// 根据位置获取周围格子
			m_grids.idaround_list(m_grids.id(aunit->x(), aunit->y()), lgrids1);
		}
		m_grids.idaround_list(lenter_pos, lgrids2);


		auto pro = std::make_shared<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>();
		pro->add_m_units(aunit->id());
		std::set<i64_actorid> lview;
		// leave 离开视野
		for (int32_t id : lgrids1)
		{
			if (lgrids2.find(id) == lgrids2.end())
			{
				grid* lpgrid = m_grids.get_grid(id);
				if (lpgrid != nullptr)
				{
					std::set<i64_actorid>* lpset = lpgrid->get_unitlist();
					std::for_each(lpset->begin(), lpset->end(), [&lview](i64_actorid item)
						{
							lview.insert(item);
						});
				}
			}
		}
		if (lview.empty() == false)
		{
			pro->set_m_isenter(false);
			actor::send_client(lview, pro);
			m_leaveview(aunit->id(), lview);
		}

		lview.clear();
		// enter 进入视野
		for (int32_t id : lgrids2)
		{
			if (lgrids1.find(id) == lgrids1.end())
			{
				grid* lpgrid = m_grids.get_grid(id);
				if (lpgrid != nullptr)
				{
					std::set<i64_actorid>* lpset = lpgrid->get_unitlist();
					std::for_each(lpset->begin(), lpset->end(), [&lview](i64_actorid item)
						{
							lview.insert(item);
						});
				}
			}
		}
		if (lview.empty() == false)
		{
			pro->set_m_isenter(true);
			actor::send_client(lview, pro);
			m_enterview(aunit->id(), lview);
		}



		aunit->set_x(ax);
		aunit->set_y(ay);

		return true;

	}

	void aoimap::leave(unit* aunit)
	{
		int lgrid = m_grids.id(aunit->x(), aunit->y());
		m_grids.leave(aunit, lgrid);

		auto pro = std::make_shared<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>();
		pro->add_m_units(aunit->id());
		pro->set_m_isenter(false);

		std::set<int32_t> lgrids;
		m_grids.idaround_list(m_grids.id(aunit->x(), aunit->y()), lgrids);
		for (int32_t id : lgrids)
		{
			grid* lpgrid = m_grids.get_grid(id);
			if (lpgrid != nullptr)
			{
				std::set<i64_actorid>* lpset = lpgrid->get_unitlist();
				actor::send_client(*lpset, pro);
			}
		}
	}

	void aoimap::sync_position(unit* aunit, int32_t agridid)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_SYNC_POSITION>();
		(*pro->mutable_m_position())[aunit->id()] = aunit->position();

		std::set<int32_t> lgrids;
		m_grids.idaround_list(agridid, lgrids);
		for (int32_t id : lgrids)
		{
			grid* lpgrid = m_grids.get_grid(id);
			if (lpgrid != nullptr)
			{
				std::set<i64_actorid>* lpset = lpgrid->get_unitlist();
				actor::send_client(*lpset, pro);
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
}