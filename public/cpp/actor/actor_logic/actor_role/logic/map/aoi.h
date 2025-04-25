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
		int32_t m_nx;  // x���������
		int32_t m_ny;  // y���������
		int32_t m_w;   // x����
		int32_t m_l;   // y�᳤��
	public:
		virtual void init(int32_t aw, int32_t al, int32_t anx, int32_t any);

		// x���������
		int32_t nx();

		// y���������
		int32_t ny();

		// ��������
		int32_t grid_count();

		// ���ӿ��
		int32_t grid_w();

		// ���ӳ���
		int32_t grid_l();

		// ͨ�����ӱ�Ż�ȡ idx
		int32_t idx(uint32_t aid);

		// ͨ�����ӱ�Ż�ȡ idy
		int32_t idy(uint32_t aid);

		// ͨ��idx,idy��ȡ ���ӱ��
		int32_t id(uint32_t aidx, uint32_t aidy);

		// ͨ��x y�����������ӱ��
		int32_t idbyxy(float ax, float ay);

		// ��ȡ������Χ��9����
		void idaround_list(int32_t aid, std::set<int32_t>& avec);

		// ���ݸ��ӱ�ż�����Χ�Ź�����
		void idaround_foreach(int32_t aid, const std::function<void(int32_t)>& afun);

		// �ڸ�����Χ���� ����ҵ����˳�
		void idaround_find(int32_t aid, const std::function<bool(int32_t)>& afun);

		// ������������
		static float distance(aoi* aaoi, int aid1, int aid2);

		// ������������
		static float distance(float ax1, float ay1, float ax2, float ay2);

		// ������������
		static float distance(const pbnet::VECTOR2& apos1, const pbnet::VECTOR2& apos2);

		// ������������
		static float distance(const pbnet::VECTOR3& apos1, const pbnet::VECTOR3& apos2);
	};
}// namespace ngl