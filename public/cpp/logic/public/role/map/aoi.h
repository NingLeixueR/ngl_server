#pragma once

#include <functional>
#include <iostream>
#include <cstdint>
#include <numeric>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <map>

#include "net.pb.h"

namespace ngl
{
	class aoi
	{
	protected:
		int32_t m_nx;  // x轴格子数量
		int32_t m_ny;  // y轴格子数量
		int32_t m_w;   // x轴宽度
		int32_t m_l;   // y轴长度
	public:
		virtual void init(int32_t aw, int32_t al, int32_t anx, int32_t any)
		{
			m_w = aw;
			m_l = al;
			m_nx = anx;
			m_ny = any;
		}

		// x轴格子数量
		int32_t nx() 
		{ 
			return m_nx; 
		}

		// y轴格子数量
		int32_t ny() 
		{ 
			return m_ny; 
		}

		// 格子数量
		inline int32_t grid_count() 
		{ 
			return m_nx * m_ny; 
		}

		// 格子宽度
		inline int32_t grid_w() 
		{ 
			return m_w / m_nx; 
		}

		// 格子长度
		inline int32_t grid_l() 
		{ 
			return m_l / m_ny; 
		}

		// 通过格子编号获取 idx
		inline int32_t idx(uint32_t aid) 
		{ 
			return aid % m_nx; 
		}

		// 通过格子编号获取 idy
		inline int32_t idy(uint32_t aid) 
		{ 
			return aid / m_nx; 
		}

		// 通过idx,idy获取 格子编号
		inline int32_t id(uint32_t aidx, uint32_t aidy) 
		{ 
			return aidy * m_nx + aidx; 
		}

		// 通过x y的坐标计算格子编号
		inline int32_t idbyxy(float ax, float ay) 
		{ 
			return id(std::ceil(ax / grid_w()), std::ceil(ay / grid_l())); 
		}

		// 获取格子周围的9宫格
		inline void idaround_list(int32_t aid, std::set<int32_t>& avec)
		{
			int lx = idx(aid);
			int ly = idy(aid);
			//左上角
			int ltx = lx - 1;
			int lty = ly - 1;
			avec.insert(aid);
			for (int i = 0; i < 3; ++i)
			{
				int ltempx = ltx + i;
				if (ltempx < 0 || ltempx >= m_nx)
					continue;
				for (int j = 0; j < 3; ++j)
				{
					int ltempy = lty + j;
					if (ltempy < 0 || ltempy >= m_ny)
						continue;
					if (ltempx != lx && ltempy != ly)
						avec.insert(id(ltempx, ltempy));
				}
			}
		}

		// 根据格子编号计算周围九宫格编号
		inline void idaround_foreach(int32_t aid, const std::function<void(int32_t)>& afun)
		{
			std::set<int32_t> lvec;
			idaround_list(aid, lvec);
			for (int32_t lid : lvec)
			{
				afun(lid);
			}
		}

		// 在格子周围查找 如果找到则退出
		inline void idaround_find(int32_t aid, const std::function<bool(int32_t)>& afun)
		{
			std::set<int32_t> lvec;
			idaround_list(aid, lvec);
			for (int32_t lid : lvec)
			{
				if (afun(lid) == true)
					return;
			}
		}

		// 计算两点间距离
		static float distance(aoi* aaoi, int aid1, int aid2)
		{
			float lx1 = aaoi->idx(aid1);
			float ly1 = aaoi->idy(aid1);
			float lx2 = aaoi->idx(aid2);
			float ly2 = aaoi->idy(aid2);
			return distance(lx1, ly1, lx2, ly2);
		}

		// 计算两点间距离
		static float distance(float ax1, float ay1, float ax2, float ay2)
		{
			float lx = ax2 - ax1;
			float ly = ay2 - ay1;
			return std::sqrt(lx * lx + ly * ly);
		}

		// 计算两点间距离
		static float distance(const pbnet::VECTOR2& apos1, const pbnet::VECTOR2& apos2)
		{
			return distance(apos1.m_x(), apos1.m_y(), apos2.m_x(), apos2.m_y());
		}

		// 计算两点间距离
		static float distance(const pbnet::VECTOR3& apos1, const pbnet::VECTOR3& apos2)
		{
			return distance(apos1.m_x(), apos1.m_y(), apos2.m_x(), apos2.m_y());
		}
	};
}// namespace ngl