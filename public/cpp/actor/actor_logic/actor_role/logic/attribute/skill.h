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
	struct skill_data
	{
		// xml con.base
		struct base
		{
			std::string m_name;			// 技能名称
			std::string m_describe;		// 技能描述
			int32_t		m_cooldown;		// 冷却时间(毫秒)
			struct lv
			{
				std::map<EnumAttribute, int32_t> m_consume; // 释放技能消耗属性值
			};
			
		};

		struct setp
		{
			struct lv
			{
				int32_t m_start;				// 开始时间:相对于技能开始时间的毫秒
				int32_t m_finish;				// 结束时间:相对于技能开始时间的毫秒
				std::set<int32_t> m_buffids;	// 附加buff
			};
			std::map<int32_t,lv> m_lvs;
		};

		base m_base;
		std::vector<setp> m_setp;

		bool load(const char* axml);
	};

	class unit;

	class skill
	{
		skill_data m_data;
		int32_t m_lv;				// 技能等级
		int32_t m_cooldownutc;		// 技能释放时间utc
		unit* m_unit;
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


		// # 检查是否可以释放技能
		virtual bool release_check()
		{
			// 获取冷却时间
			m_data.m_base.m_cooldown;

		}
	
		// # 释放技能
		virtual bool release();


	};

}//namespace ngl