/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for nmap.

#pragma once

#include "actor/actor_logic/actor_role/logic/attribute/unit.h"
#include "actor/actor_logic/actor_role/logic/nmap/grid.h"
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
	class grids : public aoi
	{
		std::vector<grid> m_grid;
	public:
		virtual void init(int32_t aw, int32_t al, int32_t anx, int32_t any);

		// # (Ax,ay)
		bool enter(i64_actorid aunitid, int32_t ax, int32_t ay);

		// Translated comment.
		bool enter(i64_actorid aunitid, int32_t agid);

		// # (Ax,ay)leave
		void leave(i64_actorid aunitid, int32_t ax, int32_t ay);

		// # Leave
		void leave(i64_actorid aunitid, int32_t agid);

		// # Get
		grid* get_grid(int32_t agid);

		// # (Ax,ay)get
		grid* get_grid(int32_t ax, int32_t ay);
	};

	struct obstacle
	{
		int m_id = 0;			// Translated comment.
		eobstacles m_pass;		// Whethercanthrough
		int m_value = 0;		// Translated comment.
		int m_parent = 0;

		void clear()
		{
			m_value = 0;
			m_parent = -1;
		}
	};

	class obstacles : public aoi
	{
	public:
		std::vector<obstacle> m_grid;

		virtual void init(int32_t aw, int32_t al, int32_t anx, int32_t any)
		{
			aoi::init(aw, al, anx, any);
			m_grid.resize(grid_count());
		}

		void set_obstacles(const std::vector<obstacles_data>& aobstacles)
		{
			int lindex = 0;
			for (const obstacles_data& item : aobstacles)
			{
				//eobstacles
				for (int32_t eitem : item.m_data)
				{
					obstacle& lobstacle = m_grid[lindex];
					lobstacle.m_id = lindex;
					lobstacle.m_pass = (eobstacles)eitem;
					++lindex;
				}				
			}
			clear();
		}

		void clear()
		{
			for (auto& item : m_grid)
			{
				item.clear();
			}
		}

		bool is_pass(int32_t aid)
		{
			return m_grid[aid].m_pass == eobstacles_pass;
		}

		bool is_pass(int32_t ax, int32_t ay)
		{
			return is_pass(id(ax, ay));
		}

		bool find_path(int32_t ax1, int32_t ay1, int32_t ax2, int32_t ay2, std::list<int>& apath)
		{
			// (Ax1, ay1)
			// (Ax2, ay2)
			std::vector<bool> vis;
			vis.resize(m_grid.size(), false);
			int lstatrt = id(ax1, ay1);
			int lfinish = id(ax2, ay2);
			int lcurrent = lstatrt;
			class lessfun
			{
			public:
				static int64_t make(int32_t aid, int32_t av)
				{
					int64_t lvalues;
					((int32_t*)&lvalues)[0] = aid;
					((int32_t*)&lvalues)[1] = av;
					return lvalues;
				}

				static int32_t value(int64_t av)
				{
					return ((int32_t*)&av)[1];
				}

				bool operator()(int64_t aid1, int64_t aid2)const
				{
					return value(aid1) < value(aid2);
				}
			};
			std::multiset<int64_t, lessfun> lset;

			vis[lcurrent] = true;

			while (true)
			{
				idaround_foreach(lcurrent, [&vis, &lset, lcurrent, lfinish, this](int aid)->void
					{
						if (vis[aid] == true)
						{
							return;
						}
						if (is_pass(aid) == false)
						{
							return;
						}
						m_grid[aid].m_value = (int)distance(this, aid, lfinish);
						m_grid[aid].m_parent = lcurrent;
						lset.insert(lessfun::make(aid, m_grid[aid].m_value));
						vis[aid] = true;
					});

				if (lset.empty())
				{
					clear();
					return false;
				}
				auto itor = lset.begin();
				lcurrent = (int)*itor;
				lset.erase(itor);

				if (lcurrent == lfinish)
				{
					do
					{
						apath.push_front(lcurrent);
						lcurrent = m_grid[lcurrent].m_parent;
					} while (lcurrent != -1);
					clear();
					return true;
				}
			}
		}

		bool find_path(int32_t aid1, int32_t aid2, std::list<int>& apath)
		{
			return find_path(idx(aid1), idy(aid1), idx(aid2), idy(aid2), apath);
		}

		std::string printf_point(int aid)
		{
			std::stringstream lstr;
			lstr << "(" << idx(aid) << "," << idy(aid) << ")";
			return lstr.str();
		}

		void printf(int32_t aid1, int32_t aid2, std::list<int>& apath)
		{
			auto lstream = log_error();
			if (apath.empty())
			{
				(*lstream) << "[" << printf_point(aid1) << "->" << printf_point(aid2) << "]" << std::endl;
				(*lstream) << "path:" << "路径不通" << std::endl;
			}
			else
			{
				(*lstream) << "[" << printf_point(aid1) << "->" << printf_point(aid2) << "]" << std::endl;
				(*lstream) << "path:" << std::endl;
				for (int lid : apath)
				{
					(*lstream) << printf_point(lid);
				}
				(*lstream) << std::endl;
				for (int x = 0; x < nx(); ++x)
				{
					(*lstream) << x << "\t";
					for (int y = 0; y < ny(); ++y)
					{
						int ltempid = id(x, y);
						if (m_grid[ltempid].m_pass == false)
						{
							(*lstream) << "#";
						}
						else
						{
							if (std::find(apath.begin(), apath.end(), ltempid) == apath.end())
							{
								(*lstream) << "@";
							}
							else
							{
								(*lstream) << "*";
							}
						}
					}
					(*lstream) << std::endl;
				}
			}
			lstream->print("");
		}

		bool print_findpath(int32_t aid1, int32_t aid2, std::list<int>& apath)
		{
			if (find_path(idx(aid1), idy(aid1), idx(aid2), idy(aid2), apath))
			{
				printf(aid1, aid2, apath);
				return true;
			}
			else
			{
				printf(aid1, aid2, apath);
				return false;
			}
		}
	};
}// namespace ngl