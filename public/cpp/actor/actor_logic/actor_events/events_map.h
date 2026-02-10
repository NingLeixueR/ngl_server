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

#include "actor_events.h"

namespace ngl
{
	// 事件枚举类型
	enum eevents_map
	{
		eevents_map_leaveview,		// 离开视野
		eevents_map_enterview,		// 进入视野
		eevents_map_death,			// 单位死亡
		eevents_map_count,
	};

	// 事件类型
	class np_eevents_map
	{
	public:
		np_eevents_map(eevents_map atype) :
			m_type(atype)
		{}

		eevents_map m_type;
	};

	class np_eevents_map_leaveview :public np_eevents_map
	{
	public:
		np_eevents_map_leaveview() :
			np_eevents_map(eevents_map_leaveview),
			m_leaveunitid(0)
		{}

		i64_actorid m_leaveunitid;
		std::set<i64_actorid> m_unitids;
		DPROTOCOL(np_eevents_map_leaveview, m_type, m_leaveunitid, m_unitids)
	};

	class np_eevents_map_enterview :public np_eevents_map
	{
	public:
		np_eevents_map_enterview() :
			np_eevents_map(eevents_map_enterview),
			m_enterunitid(0)
		{}

		i64_actorid m_enterunitid;
		std::set<i64_actorid> m_unitids;
		DPROTOCOL(np_eevents_map_enterview, m_enterunitid, m_unitids)
	};

	class np_eevents_map_death :public np_eevents_map
	{
	public:
		np_eevents_map_death() :
			np_eevents_map(eevents_map_death),
			m_deathunitid(0)
		{}

		i64_actorid m_deathunitid;
		DPROTOCOL(np_eevents_map_death, m_deathunitid)
	};

	using actor_events_map = actor_events<ENUM_EVENTS_LOGIC, eevents_map, eevents_map_count>;
}