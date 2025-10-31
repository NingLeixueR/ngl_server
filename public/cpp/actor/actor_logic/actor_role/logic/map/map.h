/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "grids.h"
#include "actor.h"

namespace ngl
{
	class aoimap
	{
		grids														m_grids;		// ��Ұ����
		obstacles													m_obstacles;	// Ѱ·����(��λС����)
		int															m_tabid;		// ��ͼ�����������
		std::map<i64_actorid, unit_role*>							m_roleunit;
		std::map<i64_actorid, unit_monster*>						m_monster;
		std::map<i64_actorid, unit_region*>							m_region;		
	public:
		aoimap();

		// # ����role
		unit_role*		find_role(i64_actorid aid);

		// # ����monster
		unit_monster*	find_monster(i64_actorid aid);

		// # ����region
		unit_region*	find_region(i64_actorid aid);

		// # ����unit
		unit*			find_unit(i64_actorid aid);

		// # ��ʼ��
		bool init(int32_t atid);

		// # ���� unit => pbnet::UNIT
		bool copy_unit(unit* aunit, pbnet::UNIT* aUNIT);

		// # ����unit id��ȡpbnet::UNIT
		bool get_unit(i64_actorid aid, pbnet::UNIT* aunit);

		virtual bool enter(unit* aunit, int32_t ax, int32_t ay);

		virtual void leave(unit* aunit);

		void sync_position(unit* aunit, int32_t agridid);

		bool move(unit* aunit, int32_t ax, int32_t ay);

		// �ı䷽��
		void change_angle(i64_actorid aroleid, int32_t aangle);

		// �ı��ٶ�
		void change_speed(i64_actorid aroleid, int32_t aspeed);
		
		void change(pbnet::UNIT_POSITION& aposition);

		void update(int64_t ams);
	};
}// namespace ngl