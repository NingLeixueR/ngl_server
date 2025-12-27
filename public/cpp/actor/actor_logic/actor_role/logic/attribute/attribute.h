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

#include "attribute_value.h"
#include "ttab_attribute.h"
#include "manage_csv.h"
#include "events_map.h"
#include "actor_base.h"
#include "net.pb.h"
#include "nlog.h"
#include "net.h"

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <map>

namespace ngl
{
	class attribute
	{
		// # 模块属性(只能模块间发生变化通过updata[EnumModule,attribute_value])
		std::map<EnumModule, attribute_value> m_moduledata;
		bool m_sync = false;
		int64_t m_unitid = 0;

		// # 根节点
		attribute_value& root();

		// # 添加模块，并将属性添加到父链
		void module_add(EnumModule aenum);

		// # 移除模块,并将其添加到父链上的属性去除
		void module_dec(EnumModule aenum);

		// # 判断EnumModule是否是根节点
		bool module_is_root(EnumModule aenum);

		// # 判断EnumModule是否为空
		bool module_is_null(EnumModule aenum);

		// # 更新模块
		void module_update(EnumModule aenum);
	public:
		attribute()
		{
		}
		
		bool sync();

		void set_sync(bool async);

		// # 初始化数据 将模块属性输入
		void init_data(EnumModule aenum, attribute_value& avalue);

		// # 计算输入的模块属性 生成最终属性与战力
		void init();

		// # 更新模块属性
		void updata(EnumModule aenum, attribute_value& avalue);

		// # 移除属性模块
		void remove(EnumModule aenum);

		// # 战力
		int64_t fight();

		const std::map<EnumAttribute, int64_t>& get_attribute();

		//EnumModule aenum
		const std::map<EnumAttribute, int64_t>& get_attribute(EnumModule aenum);

		void printf();

		template <typename T>
		void topb(T& apro)
		{
			for (std::pair<const EnumModule, attribute_value>& item : m_moduledata)
			{
				pbnet::UnitModule* lpmodule = apro.add_mmodules();
				item.second.topb(*lpmodule);
			}
		}
	};

	class aoimap;

	// 动态属性,可以在战斗中改变的属性
	class dynamic_attribute
	{
	private:
		std::map<EnumAttribute, int64_t>	m_base;
		std::map<EnumAttribute, int64_t>	m_dynamic;
		std::map<EnumFightStat, bool>		m_stat;
		int64_t m_unitid = 0;
		
	public:
		dynamic_attribute(int64_t aunitid) :
			m_unitid(aunitid)
		{
		}

		// 根据[模块属性]生成动态属性
		void init(const std::map<EnumAttribute, int64_t>& aattribute)
		{
			m_base = aattribute;
			m_dynamic = m_base;
		}

		// 是否死亡
		bool is_death()
		{
			return m_dynamic[E_Hp] <= 0;
		}

		// 修改动态属性
		void change_attribute(EnumAttribute aattribute, int64_t avalue, bool adynamic)
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

		// 是否可以移动
		bool is_move()
		{
			return get_fightstst(E_FightMove);
		}

		// 是否可以普通攻击
		bool is_normalattack()
		{
			return get_fightstst(E_FightNormalAttack);
		}

		// 是否可以释放技能
		bool is_releaseskill()
		{
			return get_fightstst(E_FightReleaseSkill);
		}
	};

	void test_attribute();
}// namespace ngl