/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for attribute.


#pragma once

#include "actor/actor_logic/actor_role/logic/attribute/attribute.h"
#include "tools/tools.h"

namespace ngl
{
	struct skill_template
	{
		std::string					m_name;			// Name
		std::string					m_describe;		// Translated comment.
		int32_t						m_maxlv;		// Level
		std::map<int32_t, int32_t>	m_cooldown;		// Time( ) key:lv value:ms
				
		struct consume_attr
		{
			std::map<ngl::EnumAttribute, int32_t> m_data;

			DXMLSERIALIZE(consume_attr, false, m_data)
		};
		std::map<int32_t, consume_attr> m_consumeattr;  // Key:lv attribute

		struct consume_item
		{
			std::map<int32_t, int32_t> m_data;	// key:itemid value:itemcount

			DXMLSERIALIZE(consume_item, false, m_data)
		};
		std::map<int32_t, consume_item> m_consumeitem;  // Key:lv item

		struct setp
		{
			struct lv
			{
				int32_t m_release;				// : Time
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
				std::set<buff> m_buffids;	// Buff

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
		skill_template	m_template;			// xml
		int32_t			m_lv;				// Level
		int32_t			m_cooldownutc;		// Timeutc
		unit*			m_unit;				// Translated comment.
	public:
		static std::shared_ptr<skill> create(int32_t askillid, int32_t askilllv)
		{
			return nullptr;
		}

		// # [Set/get] level
		bool set_lv(int32_t askilllv);
		int32_t lv();

		// # Get time
		int32_t cooldown();

		// # Whethercan
		virtual bool release_check();
	
		// Translated comment.
		virtual bool release()
		{
			return true;
		}
	};
}//namespace ngl