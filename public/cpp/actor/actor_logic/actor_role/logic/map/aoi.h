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
		virtual void init(int32_t aw, int32_t al, int32_t anx, int32_t any);

		// x轴格子数量
		int32_t nx();

		// y轴格子数量
		int32_t ny();

		// 格子数量
		int32_t grid_count();

		// 格子宽度
		int32_t grid_w();

		// 格子长度
		int32_t grid_l();

		// 通过格子编号获取 idx
		int32_t idx(uint32_t aid);

		// 通过格子编号获取 idy
		int32_t idy(uint32_t aid);

		// 通过idx,idy获取 格子编号
		int32_t id(uint32_t aidx, uint32_t aidy);

		// 通过x y的坐标计算格子编号
		int32_t idbyxy(float ax, float ay);

		// 获取格子周围的9宫格
		void idaround_list(int32_t aid, std::set<int32_t>& avec);

		// 根据格子编号计算周围九宫格编号
		void idaround_foreach(int32_t aid, const std::function<void(int32_t)>& afun);

		// 在格子周围查找 如果找到则退出
		void idaround_find(int32_t aid, const std::function<bool(int32_t)>& afun);

		// 计算两点间距离
		static float distance(aoi* aaoi, int aid1, int aid2);

		// 计算两点间距离
		static float distance(float ax1, float ay1, float ax2, float ay2);

		// 计算两点间距离
		static float distance(const pbnet::VECTOR2& apos1, const pbnet::VECTOR2& apos2);

		// 计算两点间距离
		static float distance(const pbnet::VECTOR3& apos1, const pbnet::VECTOR3& apos2);
	};
}// namespace ngl