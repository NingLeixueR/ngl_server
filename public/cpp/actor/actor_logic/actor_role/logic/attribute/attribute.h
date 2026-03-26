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

#include "actor/actor_logic/actor_role/logic/attribute/attribute_value.h"
#include "actor/actor_logic/actor_events/events_map.h"
#include "actor/actor_base/core/actor_base.h"
#include "actor/tab/ttab_attribute.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/generated/pb/net.pb.h"
#include "tools/log/nlog.h"
#include "net/tcp/ntcp.h"

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <map>

namespace ngl
{
	class attribute
	{
		// # Moduleattribute( module changethroughupdata[EnumModule,attribute_value])
		std::map<EnumModule, attribute_value> m_moduledata;
		bool m_sync = false;
		int64_t m_unitid = 0;

		// # Node
		attribute_value& root();

		// # Addmodule, and attributeaddto
		void module_add(EnumModule aenum);

		// # Removemodule,and addto on attribute
		void module_dec(EnumModule aenum);

		// # CheckEnumModulewhether node
		bool module_is_root(EnumModule aenum);

		// # CheckEnumModulewhether
		bool module_is_null(EnumModule aenum);

		// # Module
		void module_update(EnumModule aenum);
	public:
		attribute() = default;
		
		bool sync();

		void set_sync(bool async);

		// # Initializedata moduleattribute
		void init_data(EnumModule aenum, attribute_value& avalue);

		// # Moduleattribute attributeand
		void init();

		// # Moduleattribute
		void updata(EnumModule aenum, attribute_value& avalue);

		// # Removeattributemodule
		void remove(EnumModule aenum);

		// Translated comment.
		int64_t fight();

		const std::map<EnumAttribute, int64_t>& get_attribute();

		//EnumModule aenum
		const std::map<EnumAttribute, int64_t>& get_attribute(EnumModule aenum);

		void printf();

		template <typename T>
		void topb(T& apro)
		{
			for (auto& lpair : m_moduledata)
			{
				pbdb::UNIT_MODULE* lpmodule = apro.add_mmodules();
				lpair.second.topb(*lpmodule);
			}
		}
	};

	class aoimap;

	// Attribute,can in attribute
	class dynamic_attribute
	{
	private:
		std::map<EnumAttribute, int64_t>	m_base;
		std::map<EnumAttribute, int64_t>	m_dynamic;
		std::map<EnumFightStat, bool>		m_stat;
		int64_t m_unitid = 0;
		
	public:
		explicit dynamic_attribute(int64_t aunitid) :
			m_unitid(aunitid)
		{
		}

		// [Moduleattribute] attribute
		void init(const std::map<EnumAttribute, int64_t>& aattribute)
		{
			m_base = aattribute;
			m_dynamic = m_base;
		}

		// Whether
		bool is_death()
		{
			return m_dynamic[E_Hp] <= 0;
		}

		// Attribute
			void change_attribute(EnumAttribute aattribute, int64_t avalue, [[maybe_unused]] bool adynamic)
		{
			m_dynamic[aattribute] += avalue;
			if (is_death())
			{
				np_eevents_map_death lparm;
				lparm.m_deathunitid = m_unitid;
				actor_events_map::trigger_event(lparm);
			}
		}

		void set_attribute(EnumAttribute aattribute, int64_t avalue)
		{
			m_dynamic[aattribute] = avalue;
			if (is_death())
			{
				np_eevents_map_death lparm;
				lparm.m_deathunitid = m_unitid;
				actor_events_map::trigger_event(lparm);
			}
		}

		int64_t get_attribute(EnumAttribute aattribute)
		{
			return m_dynamic[aattribute];
		}

		bool get_fightstst(EnumFightStat astat)
		{
			return m_stat[astat];
		}

		// Whethercan
		bool is_move()
		{
			return get_fightstst(E_FightMove);
		}

		// Whethercan
		bool is_normalattack()
		{
			return get_fightstst(E_FightNormalAttack);
		}

		// Whethercan
		bool is_releaseskill()
		{
			return get_fightstst(E_FightReleaseSkill);
		}
	};

	void test_attribute();
}// namespace ngl
