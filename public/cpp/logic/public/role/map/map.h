#pragma once

#include "grids.h"
#include "actor.h"

namespace ngl
{
	class aoimap
	{
		grids														m_grids;		// 视野格子
		obstacles													m_obstacles;	// 寻路格子(单位小格子)
		int															m_tabid;		// 地图表格配置数据
		std::map<i64_actorid, unit_role*>							m_roleunit;
		std::map<i64_actorid, unit_monster*>						m_monster;
		std::map<i64_actorid, unit_region*>							m_region;		
	public:
		aoimap();

		// # 查找role
		unit_role*		find_role(i64_actorid aid);

		// # 查找monster
		unit_monster*	find_monster(i64_actorid aid);

		// # 查找region
		unit_region*	find_region(i64_actorid aid);

		// # 查找unit
		unit*			find_unit(i64_actorid aid);

		// # 初始化
		bool init(int32_t atid);

		// # 拷贝 unit => pbnet::UNIT
		bool copy_unit(unit* aunit, pbnet::UNIT* aUNIT);

		// # 根据unit id获取pbnet::UNIT
		bool get_unit(i64_actorid aid, pbnet::UNIT* aunit);

		virtual bool enter(unit* aunit, int32_t ax, int32_t ay);

		virtual void leave(unit* aunit);

		void sync_position(unit* aunit, int32_t agridid);

		bool move(unit* aunit, int32_t ax, int32_t ay);

		// 改变方向
		void change_angle(i64_actorid aroleid, int32_t aangle);

		// 改变速度
		void change_speed(i64_actorid aroleid, int32_t aspeed);
		
		void change(pbnet::UNIT_POSITION& aposition);

		void update(int64_t ams);
	};
}// namespace ngl