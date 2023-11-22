#pragma once 

#include <set>
#include <map>
#include <vector>
#include <array>
#include <functional>
#include "net.h"
#include "nlog.h"
#include "manage_csv.h"
#include "ttab_attribute.h"
#include "attribute_value.h"

namespace ngl
{
	class attribute
	{
		std::map<EnumModule, attribute_value> m_moduledata;

		attribute_value& root();

		void add_module(EnumModule aenum);
		void dec_module(EnumModule aenum);

		bool is_module_root(EnumModule aenum);
		bool is_module_null(EnumModule aenum);

		void update_module(EnumModule aenum);
	public:
		// #### 初始化数据 将模块属性输入
		void init_data(EnumModule aenum, attribute_value& avalue);
		// #### 计算输入的模块属性 生成最终属性与战力
		void init();

		void updata(EnumModule aenum, attribute_value& avalue);

		int64_t fight();

		const map_attr& get_attribute();

		//EnumModule aenum
		const map_attr& get_attribute(EnumModule aenum);

		void printf();
	};

	void test_attribute();
}