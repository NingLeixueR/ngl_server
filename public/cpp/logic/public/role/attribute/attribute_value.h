#pragma once

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <map>

#include "ttab_attribute.h"
#include "manage_csv.h"
#include "nlog.h"
#include "net.h"

namespace ngl
{
	struct attribute_value
	{
		friend class attribute;
	private:
		// ### [absolute]		属性绝对值  
		map_absolute m_attr;
		// ### 万分比属性[自身提供的]
		map_ratio m_rattr;
		// ### 万分比属性[别人提供的]
		map_ratio m_orattr;
		// ########### 以下比例属性 是往父链上添加的属性 ##########
		// ### key:哪个模块加的比例属性
		map_moduleratio m_crattr;		// 往父链的[万分比属性]上添加 
		// #### [m_attr+m_rattr] 产生的属性
		map_attr m_fight;
		// ### 战力
		int64_t m_fightscore;

		EnumModule m_module;
	public:
		attribute_value();
	private:
		void update(map_attribute& aattr, const map_ratio& amr);

		int64_t fight(EnumAttribute atype, double avalues);

		int64_t fight();
	public:
		map_attr& get_fight()
		{
			return m_fight;
		}
		int64_t update();
		// 打印属性
		void printf();
		// 清空属性
		void clear();
		// 添加属性
		void set_attr(EnumAttribute atype, double avalues);
		// 添加比例属性属性
		void set_rattr(EnumAttribute atype, double avalues);
		// 给父结点添加属性
		void set_father_rattr(EnumModule amodule, EnumAttribute atype, double avalues);
		// 获取属性
		double get_attr(EnumAttribute atype);
		// 获取比例属性属性
		double get_rattr(EnumAttribute atype);
		// 获取父结点添属性
		double get_father_rattr(EnumModule amodule, EnumAttribute atype);

		void topb(pbnet::UnitModule& aunitmodule)
		{
			aunitmodule.set_m_mtype(m_module);
			for (std::pair<const EnumAttribute, int64_t>& item : m_fight)
			{
				pbnet::UnitAttribute* lptemp = aunitmodule.add_m_modules();
				lptemp->set_m_type(item.first);
				lptemp->set_m_value(item.second);
			}
		}
	};
}// namespace ngl