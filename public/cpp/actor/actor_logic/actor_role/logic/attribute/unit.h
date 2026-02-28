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

#include "actor/actor_logic/actor_role/logic/attribute/attribute.h"
#include "actor/pb/net.pb.h"
#include "actor/pb/db.pb.h"

namespace ngl
{
	class unit
	{
		unit() = delete;

		pbdb::EUNIT					m_type = pbdb::EUNIT::EUNIT_FAIL;
		i64_actorid					m_id = nguid::make();
		attribute					m_attribute;
		dynamic_attribute			m_dynamic;
		pbdb::POSITION				m_position;
	public:
		unit(i64_actorid aunitid, pbdb::EUNIT atype) :
			m_type(atype),
			m_dynamic(aunitid)
		{}

		void init(attribute aattribute)
		{
			m_attribute.get_attribute();
			m_dynamic.init(m_attribute.get_attribute());
		}

		// # [设置/获取]unit类型
		bool set_type(pbdb::EUNIT atype);
		pbdb::EUNIT type();

		// # 获取unit属性
		attribute& get_attribute();

		// # [设置/获取]unit id
		bool set_id(int32_t aid);
		int32_t id();

		// # 获取unit动态属性
		dynamic_attribute& dynamic();

		// # [设置/获取]unit position
		void set_x(int32_t ax);
		void set_y(int32_t ay);
		int32_t x();
		int32_t y();
		pbdb::POSITION& position();
		bool is_position_invalid();

		// # [设置/获取]unit 方向
		void set_angle(int32_t aangle);
		int32_t angle();

		// # [设置/获取]unit 速度
		void set_speed(int32_t aspeed);
		int32_t speed();

		void update(int64_t ams);
	};

	class unit_role: public unit
	{
	public:
		unit_role(i64_actorid aunitid) :
			unit(aunitid, pbdb::EUNIT::EUNIT_ROLE)
		{}
	};

	class unit_monster : public unit
	{
	public:
		unit_monster(i64_actorid aunitid):
			unit(aunitid, pbdb::EUNIT::EUNIT_MONSTER)
		{}
	};
}// namespace ngl