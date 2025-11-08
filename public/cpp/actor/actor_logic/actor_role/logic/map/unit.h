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

#include "attribute.h"
#include "net.pb.h"

namespace ngl
{
	class unit
	{
		pbnet::eunit			m_type;		
		attribute				m_attribute;
		int64_t					m_id = 0;
		pbnet::UNIT_POSITION	m_position;
		int64_t					m_lastupdate = 0;
	public:
		explicit unit(pbnet::eunit atype, int64_t aid);

		attribute& get_attribute();

		int64_t id();
		pbnet::eunit type();

		int32_t x();
		int32_t y();
		int32_t angle();
		int32_t speed();

		void set_x(int32_t ax);
		void set_y(int32_t ay);
		void set_angle(int32_t aangle);
		void set_speed(int32_t aspeed);

		pbnet::UNIT_POSITION& position();
		void set_position_invalid();
		bool is_position_invalid();

		void update(int64_t alastupdate, pbnet::VECTOR2& apos);
	};

	class unit_role : public unit
	{
	public:
		explicit unit_role(int64_t aid) :
			unit(pbnet::eunit_role, aid)
		{}
	};

	class unit_monster : public unit
	{
	public:
		explicit unit_monster(int64_t aid) :
			unit(pbnet::eunit_monster, aid)
		{}
	};

	class unit_region : public unit
	{
	public:
		explicit unit_region(int64_t aid) :
			unit(pbnet::eunit_region, aid)
		{}
	};
}// namespace ngl