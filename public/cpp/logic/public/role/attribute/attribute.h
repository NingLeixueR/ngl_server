#pragma once 

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <map>

#include "attribute_value.h"
#include "ttab_attribute.h"
#include "manage_csv.h"
#include "net.pb.h"
#include "event.h"
#include "nlog.h"
#include "net.h"

namespace ngl
{
	class attribute
	{
		// 模块属性(只能模块间发生变化通过updata[EnumModule,attribute_value])
		std::map<EnumModule, attribute_value> m_moduledata;
		bool m_issync;

		attribute_value& root();

		void add_module(EnumModule aenum);
		void dec_module(EnumModule aenum);

		bool is_module_root(EnumModule aenum);
		bool is_module_null(EnumModule aenum);

		void update_module(EnumModule aenum);
	public:
		attribute();
		
		bool issync();

		void set_issync(bool aissync);

		// #### 初始化数据 将模块属性输入
		void init_data(EnumModule aenum, attribute_value& avalue);

		// #### 计算输入的模块属性 生成最终属性与战力
		void init();

		// #### 更新模块属性
		void updata(EnumModule aenum, attribute_value& avalue);

		// #### 战力
		int64_t fight();

		const map_attr& get_attribute();

		//EnumModule aenum
		const map_attr& get_attribute(EnumModule aenum);

		void printf();

		template <typename T>
		void topb(T& apro)
		{
			for (std::pair<const EnumModule, attribute_value>& item : m_moduledata)
			{
				pbnet::UnitModule* lpmodule = apro.add_m_modules();
				item.second.topb(*lpmodule);
			}
		}
	};
	class aoimap;
	// 动态属性,可以在战斗中改变的属性
	class dynamic_attribute
	{
	public:
	private:
		std::map<EnumAttribute, int64_t> m_dynamic;
		aoimap* m_map;
		int64_t m_unitid;
		int32_t m_stat;	// 状态	 
	public:
		dynamic_attribute(aoimap* amap, int64_t aunitid) :
			m_map(amap),
			m_unitid(aunitid),
			m_stat(pbnet::eunitstat_normal)
		{}

		// 根据[模块属性]生成动态属性
		void init_dynamic(std::map<EnumModule, attribute_value>& amoduledata)
		{
			m_dynamic = amoduledata[EnumModule::E_ModuleRoot].get_fight();
		}

		// 是否死亡
		bool is_death()
		{
			return m_dynamic[E_Hp] <= 0;
		}

		// 修改动态属性
		void change_attribute(EnumAttribute aattribute, int32_t avalue)
		{
			m_dynamic[aattribute] += avalue;
			if (is_death())
			{
				event_parm_death pram;
				pram.m_unitid = -1;
				pram.m_deathunitid = m_unitid;
				event_death::execute(&pram);
				//mapevent::on_death(m_map, m_unitid);
			}
		}

		// 无法移动
		void set_stat(pbnet::eunitstat astat)
		{
			m_stat |= (int32_t)astat;
		}

		void clear_stat()
		{
			m_stat = pbnet::eunitstat_normal;
		}

		// 是否可以移动
		bool ismove()
		{
			return (m_stat & (int32_t)pbnet::eunitstat_nomove) == 0;
		}

		// 是否可以普通攻击
		bool isnormalattack()
		{
			return (m_stat & (int32_t)pbnet::eunitstat_nonormalattack) == 0;
		}

		// 是否可以释放技能
		bool isreleaseskill()
		{
			return (m_stat & (int32_t)pbnet::eunitstat_noreleaseskill) == 0;
		}
	};

	void test_attribute();
}// namespace ngl