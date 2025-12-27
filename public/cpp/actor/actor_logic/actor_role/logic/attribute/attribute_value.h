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

#include "ttab_attribute.h"
#include "manage_csv.h"
#include "nlog.h"
#include "net.h"

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <map>

namespace ngl
{
	class enum_attr_name
	{
	public:
		static void init();
		static std::pair<const char*, const char*> name(EnumAttribute atype);
		static EnumAttribute type(const char* aname);
		static EnumAttribute null_type();
	};

	class tools_attribute
	{
	public:
		static void add(std::map<EnumAttribute, int64_t>& al, const std::map<EnumAttribute, int64_t>& ar);
		static void add(std::map<EnumAttribute, float>& al, const std::map<EnumAttribute, float>& ar);
		static void dec(std::map<EnumAttribute, int64_t>& al, const std::map<EnumAttribute, int64_t>& ar);
		static void dec(std::map<EnumAttribute, float>& al, const std::map<EnumAttribute, float>& ar);

		static int64_t fight(EnumAttribute atype, double avalues);
	};

	class attribute_value
	{
		friend class attribute;
		std::map<EnumAttribute, int64_t>	m_attr;		// [absolute] 属性绝对值  
		std::map<EnumAttribute, float>		m_rattr;	// 万分比属性[自身提供的]
		std::map<EnumAttribute, float>		m_orattr;	// 万分比属性[别人提供的]
		
		// ## 以下比例属性 是往父链上添加的属性 ##
		std::map<EnumAttribute, int64_t>	m_fight;						// [m_attr+m_rattr] 产生的属性
		int64_t								m_fightscore = 0;				// 战力
		EnumModule							m_module = EnumModule::E_ModuleNull;
		std::map<EnumModule, std::map<EnumAttribute, float>> m_crattr;		// 哪个模块加的比例属性,往父链的[万分比属性]上添加 
	private:
		void update(std::map<EnumAttribute, int64_t>& aattr, const std::map<EnumAttribute, float>& amr);

		int64_t fight();
	public:
		attribute_value() = default;

		std::map<EnumAttribute, int64_t>& get_fight();

		int64_t update();

		// 打印属性
		void printf();

		// 清空属性
		void clear();

		// 添加属性
		void set_attr(EnumAttribute atype, double avalues);

		// 获取属性
		double get_attr(EnumAttribute atype);

		// 添加比例属性属性
		void set_rattr(EnumAttribute atype, double avalues);

		// 获取比例属性属性
		double get_rattr(EnumAttribute atype);

		// 给父结点添加属性
		void set_father_rattr(EnumModule amodule, EnumAttribute atype, double avalues);

		// 获取父结点添属性
		double get_father_rattr(EnumModule amodule, EnumAttribute atype);

		// 战力积分
		int64_t fightscore();

		// 转换为pb结构
		void topb(pbnet::UnitModule& aunitmodule);
	};
}// namespace ngl

template <>
struct std::formatter<ngl::EnumAttribute>
{
	constexpr auto parse(const std::format_parse_context& ctx)const
	{
		return ctx.begin();
	}

	auto format(const ngl::EnumAttribute aval, std::format_context& ctx)const
	{
		return std::format_to(ctx.out(), "{}", ngl::enum_attr_name::name(aval));
	}
};