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

#include "actor/actor_logic/actor_role/logic/attribute/unit.h"
#include "actor/actor_logic/actor_role/logic/nmap/aoi.h"
#include "actor/tab/csvtable.h"
#include "actor/pb/net.pb.h"
#include "tools/type.h"

#include <vector>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	class grid
	{
		std::set<i64_actorid> m_unitlist;
	public:
		// # 进入格子
		bool enter(i64_actorid aunitid);

		// # 离开格子
		void leave(i64_actorid aunitid);

		// # 获取unit列表
		const std::set<i64_actorid>& get_unitlist();

		void clear();

		int32_t count();
	};
}//namespace ngl