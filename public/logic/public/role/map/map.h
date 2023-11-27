#pragma once

#include "grids.h"
#include "actor.h"

namespace ngl
{
	class aoimap
	{
		grids m_grids;				// 视野格子
		obstacles m_obstacles;		// 寻路格子(单位小格子)
		int m_tabid;				// 地图表格配置数据
		std::map<i64_actorid, unit_role*> m_roleunit;
		std::map<i64_actorid, unit_monster*> m_monster;
		std::map<i64_actorid, unit_region*> m_region;
		std::function<void(i64_actorid, std::set<i64_actorid>&)> m_enterview;
		std::function<void(i64_actorid, std::set<i64_actorid>&)> m_leaveview;
	public:
		aoimap();

		// 查找unit
		unit_role*		find_role(i64_actorid aid);
		unit_monster*	find_monster(i64_actorid aid);
		unit_region*	find_region(i64_actorid aid);
		unit*			find_unit(i64_actorid aid);
		// 初始化
		bool init(int32_t atid);

		// 拷贝 unit => pbnet::UNIT
		bool copy_unit(unit* aunit, pbnet::UNIT* aUNIT);
		bool get_unit(i64_actorid aid, pbnet::UNIT* aunit);

		void set_enterview(const std::function<void(i64_actorid, std::set<i64_actorid>&)>& afun);

		void set_leaveview(const std::function<void(i64_actorid, std::set<i64_actorid>&)>& afun);


		virtual bool enter(unit* aunit, int32_t ax, int32_t ay);

		virtual void leave(unit* aunit);

		void sync_position(unit* aunit, int32_t agridid);

		bool move(unit* aunit, int32_t ax, int32_t ay);
	};
}