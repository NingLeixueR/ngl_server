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
#include "tools.h"

namespace ngl
{
	struct skill_template
	{
		std::string					m_name;			// 技能名称
		std::string					m_describe;		// 技能描述
		int32_t						m_maxlv;		// 最大等级
		std::map<int32_t, int32_t>	m_cooldown;		// 冷却时间(毫秒) key:lv value:ms
				
		struct consume_attr
		{
			std::map<ngl::EnumAttribute, int32_t> m_data;

			DXMLSERIALIZE(consume_attr, false, m_data)
		};
		std::map<int32_t, consume_attr> m_consumeattr;  // key:lv 释放技能消耗属性值

		struct consume_item
		{
			std::map<int32_t, int32_t> m_data;	// key:itemid value:itemcount

			DXMLSERIALIZE(consume_item, false, m_data)
		};
		std::map<int32_t, consume_item> m_consumeitem;  // key:lv 释放技能消耗物品道具

		struct setp
		{
			struct lv
			{
				int32_t m_release;				// 释放时刻:相对于技能开始时间的毫秒
				struct buff
				{
					int32_t m_id;
					int32_t m_lv;

					auto operator<=>(const buff& r)const
					{
						return tools::less_member(m_id, r.m_id, m_lv, r.m_lv);
					}

					DXMLSERIALIZE(buff, true, m_id, m_lv)
				};
				std::set<buff> m_buffids;	// 附加buff

				DXMLSERIALIZE(lv, false, m_release, m_buffids)
			};
			std::map<int32_t, lv> m_lvs;

			DXMLSERIALIZE(setp, false, m_lvs)
		};
		std::vector<setp> m_setp;

		DXMLSERIALIZE(skill_template, false, m_name, m_describe, m_maxlv, m_cooldown, m_consumeattr, m_consumeitem, m_setp)
	};


	class unit;

	class skill
	{
		skill_template	m_template;			// 技能xml
		int32_t			m_lv;				// 技能等级
		int32_t			m_cooldownutc;		// 技能释放时间utc
		unit*			m_unit;				// 战斗单位
	public:
		static std::shared_ptr<skill> create(int32_t askillid, int32_t askilllv)
		{
			return nullptr;
		}

		// # [设置/获取]技能等级
		bool set_lv(int32_t askilllv);
		int32_t lv();

		// # 获取技能冷却时间
		int32_t cooldown();

		// # 检查是否可以释放技能
		virtual bool release_check();
	
		// # 释放技能
		virtual bool release()
		{
			return true;
		}
	};
}//namespace ngl