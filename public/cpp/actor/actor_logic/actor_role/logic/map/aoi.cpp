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
#include "aoi.h"

namespace ngl
{
	void aoi::init(int32_t aw, int32_t al, int32_t anx, int32_t any)
	{
		m_w = aw;
		m_l = al;
		m_nx = anx;
		m_ny = any;
	}

	int32_t aoi::nx()
	{
		return m_nx;
	}

	int32_t aoi::ny()
	{
		return m_ny;
	}

	int32_t aoi::grid_count()
	{
		return m_nx * m_ny;
	}

	int32_t aoi::grid_w()
	{
		return m_w / m_nx;
	}

	int32_t aoi::grid_l()
	{
		return m_l / m_ny;
	}

	int32_t aoi::idx(uint32_t aid)
	{
		return aid % m_nx;
	}

	int32_t aoi::idy(uint32_t aid)
	{
		return aid / m_nx;
	}

	int32_t aoi::id(uint32_t aidx, uint32_t aidy)
	{
		return aidy * m_nx + aidx;
	}

	int32_t aoi::idbyxy(float ax, float ay)
	{
		return id((uint32_t)std::ceil(ax / grid_w()), (uint32_t)std::ceil(ay / grid_l()));
	}

	void aoi::idaround_list(int32_t aid, std::set<int32_t>& avec)
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
			{
				continue;
			}
			for (int j = 0; j < 3; ++j)
			{
				int ltempy = lty + j;
				if (ltempy < 0 || ltempy >= m_ny)
				{
					continue;
				}
				if (ltempx != lx && ltempy != ly)
				{
					avec.insert(id(ltempx, ltempy));
				}
			}
		}
	}

	void aoi::idaround_foreach(int32_t aid, const std::function<void(int32_t)>& afun)
	{
		std::set<int32_t> lvec;
		idaround_list(aid, lvec);
		for (int32_t lid : lvec)
		{
			afun(lid);
		}
	}

	void aoi::idaround_find(int32_t aid, const std::function<bool(int32_t)>& afun)
	{
		std::set<int32_t> lvec;
		idaround_list(aid, lvec);
		for (int32_t lid : lvec)
		{
			if (afun(lid) == true)
			{
				return;
			}
		}
	}

	float aoi::distance(aoi* aaoi, int aid1, int aid2)
	{
		return distance((float)aaoi->idx(aid1), (float)aaoi->idy(aid1), (float)aaoi->idx(aid2), (float)aaoi->idy(aid2));
	}

	float aoi::distance(float ax1, float ay1, float ax2, float ay2)
	{
		float lx = ax2 - ax1;
		float ly = ay2 - ay1;
		return std::sqrt(lx * lx + ly * ly);
	}

	float aoi::distance(const pbnet::VECTOR2& apos1, const pbnet::VECTOR2& apos2)
	{
		return distance((float)apos1.mx(), (float)apos1.my(), (float)apos2.mx(), (float)apos2.my());
	}

	float aoi::distance(const pbnet::VECTOR3& apos1, const pbnet::VECTOR3& apos2)
	{
		return distance((float)apos1.mx(), (float)apos1.my(), (float)apos2.mx(), (float)apos2.my());
	}
}//namespace ngl