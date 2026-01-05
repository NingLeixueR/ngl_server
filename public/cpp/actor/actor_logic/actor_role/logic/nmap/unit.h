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
	enum eunit
	{
		eunit_null = 0,
		eunit_role = 1,
		eunit_monster = 2,
	};

	class unit
	{
		eunit					m_type = eunit_null;
		i64_actorid				m_id = nguid::make();
		attribute				m_attribute;
		dynamic_attribute		m_dynamic;
		


	public:
		void init(attribute aattribute)
		{
			m_attribute.get_attribute();
			m_dynamic.init(m_attribute.get_attribute());
		}

		bool set_type(eunit atype);
		eunit type();

		attribute& get_attribute();

		bool set_id(int32_t aid);
		int32_t id();

		dynamic_attribute& dynamic();
	};

	class unit_role: public unit
	{};

	class unit_monster : public unit
	{};
}// namespace ngl