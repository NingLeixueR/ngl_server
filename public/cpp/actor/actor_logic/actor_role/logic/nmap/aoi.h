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

#include "actor/generated/pb/net.pb.h"

#include <functional>
#include <iostream>
#include <cstdint>
#include <numeric>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <map>

namespace ngl
{
	class aoi
	{
	protected:
		int32_t m_nx;  // X
		int32_t m_ny;  // Y
		int32_t m_w;   // X
		int32_t m_l;   // Y
	public:
		virtual void init(int32_t aw, int32_t al, int32_t anx, int32_t any);

		// X
		int32_t nx();

		// Y
		int32_t ny();

		// Translated comment.
		int32_t grid_count();

		// Translated comment.
		int32_t grid_w();

		// Translated comment.
		int32_t grid_l();

		// Through get idx
		int32_t idx(uint32_t aid);

		// Through get idy
		int32_t idy(uint32_t aid);

		// Throughidx,idyget
		int32_t id(uint32_t aidx, uint32_t aidy);

		// Throughx y
		int32_t idbyxy(float ax, float ay);

		// Get 9
		void idaround_list(int32_t aid, std::set<int32_t>& avec);

		// Translated comment.
		void idaround_foreach(int32_t aid, const std::function<void(int32_t)>& afun);

		// Find if to exit
		void idaround_find(int32_t aid, const std::function<bool(int32_t)>& afun);

		// Compute the distance between two points.
		static float distance(aoi* aaoi, int aid1, int aid2);

		// Compute the distance between two points.
		static float distance(float ax1, float ay1, float ax2, float ay2);

		// Compute the distance between two points.
		static float distance(const pbdb::VECTOR2& apos1, const pbdb::VECTOR2& apos2);

		// Compute the distance between two points.
		static float distance(const pbdb::VECTOR3& apos1, const pbdb::VECTOR3& apos2);
	};
}// namespace ngl