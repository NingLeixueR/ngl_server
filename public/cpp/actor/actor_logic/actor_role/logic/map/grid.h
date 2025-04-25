#pragma once

#include "csvtable.h"
#include "net.pb.h"
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
		// # �������
		bool enter(unit* aunit);

		// # �뿪����
		void leave(unit* aunit);

		// # ��ȡunit�б�
		std::set<i64_actorid>* get_unitlist();
	};

}//namespace ngl