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
		int32_t					m_fightid = -1;
		eunit					m_type = eunit_null;
		int32_t					m_id = nguid::none_actordataid();
		i16_area				m_area = nguid::none_area();
		attribute				m_attribute;
		dynamic_attribute		m_dynamic;
		
	public:
		void init(attribute aattribute)
		{
			m_attribute.get_attribute();
			m_dynamic.init(m_attribute.get_attribute());
		}

		bool set_fightid(int32_t afightid);
		int32_t get_fightid();

		bool set_type(eunit atype);
		eunit get_type();

		attribute& get_attribute();

		bool set_id(int32_t aid);
		int32_t get_id();

		bool set_area(i16_area aarea);
		i16_area get_area();

		// # 只有玩家的战斗单位才有[i64_actorid]
		i64_actorid id_guid();

		dynamic_attribute& dynamic();
	};
}// namespace ngl