#pragma once

#include "csvtable.h"
#include "net.pb.h"
#include "splite.h"
#include "type.h"
#include "unit.h"
#include "aoi.h"

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
		bool enter(unit* aunit);

		// # 离开格子
		void leave(unit* aunit);

		// # 获取unit列表
		std::set<i64_actorid>* get_unitlist();
	};

}//namespace ngl