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
#pragma once

#include "actor/actor_logic/actor_role/logic/nmap/grids.h"
#include "actor/actor_base/actor.h"

namespace ngl
{
	class aoimap
	{
		grids											m_grids;		// 视野格子
		obstacles										m_obstacles;	// 寻路格子(单位小格子)
		int												m_tabid;		// 地图表格配置数据
		std::map<i64_actorid, unit_role*>				m_roleunit;		// 玩家单位
		std::map<i64_actorid, unit_monster*>			m_monster;		// 怪物单位
	public:
		aoimap();

		// # 查找role
		unit_role*		find_role(i64_actorid aid);

		// # 查找monster
		unit_monster*	find_monster(i64_actorid aid);

		// # 查找unit
		unit*			find_unit(i64_actorid aid);

		// # 初始化
		bool init(int32_t atid);

		// # 拷贝 unit => pbdb::UNIT
		bool copy_unit(unit* aunit, pbdb::UNIT* aUNIT);

		// # 根据unit id获取pbnet::UNIT
		bool copy_unit(i64_actorid aid, pbdb::UNIT* aunit);

		// # 进入地图
		virtual bool enter(unit* aunit, int32_t ax, int32_t ay);

		// # 离开地图
		virtual void leave(unit* aunit);

		// # 同步位置信息
		void sync_position(unit* aunit, int32_t agridid);

		// # 移动
		bool move(unit* aunit, int32_t ax, int32_t ay);

		// # 改变方向
		void change_angle(i64_actorid aunitid, int32_t aangle);

		// # 改变速度
		void change_speed(i64_actorid aunitid, int32_t aspeed);
		
		// # 改变position
		void change(i64_actorid aunitid, pbdb::POSITION& aposition);

		void update(int64_t ams);
	};
}// namespace ngl