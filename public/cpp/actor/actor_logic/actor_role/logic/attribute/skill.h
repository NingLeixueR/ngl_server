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

namespace ngl
{
	struct skill_template
	{
		std::string					m_name;			// 技能名称
		std::string					m_describe;		// 技能描述
		std::map<int32_t, int32_t>	m_cooldown;		// 冷却时间(毫秒) key:lv value:ms
		
		struct consume
		{
			std::map<ngl::EnumAttribute, int32_t> m_data;

			xmlserialize(consume, false, m_data)
		};
		std::map<int32_t, consume> m_consume;  // 释放技能消耗属性值

		struct setp
		{
			struct lv
			{
				int32_t m_start;				// 开始时间:相对于技能开始时间的毫秒
				int32_t m_finish;				// 结束时间:相对于技能开始时间的毫秒
				std::set<int32_t> m_buffids;	// 附加buff

				xmlserialize(lv, false, m_start, m_finish, m_buffids)
			};
			std::map<int32_t, lv> m_lvs;

			xmlserialize(setp, false, m_lvs)
		};
		std::vector<setp> m_setp;

		xmlserialize(skill_template, false, m_name, m_describe, m_cooldown, m_consume, m_setp)
	};


	class unit;

	class skill
	{
		skill_template	m_template;			// 技能xml
		int32_t			m_lv;				// 技能等级
		int32_t			m_cooldownutc;		// 技能释放时间utc
		unit*			m_unit;				// 战斗单位
	public:
		static std::shared_ptr<skill> create(int32_t askillid, int32_t askilllv);

		bool set_lv(int32_t askilllv)
		{
			if (askilllv <= 0)
			{
				log_error()->print("skill::set_lv({}) fail", askilllv);
			}
			m_lv = askilllv;
		}

		int32_t get_lv()
		{
			return m_lv;
		}

		int32_t cooldown();


		// # 检查是否可以释放技能
		virtual bool release_check();
	
		// # 释放技能
		virtual bool release();


	};

}//namespace ngl