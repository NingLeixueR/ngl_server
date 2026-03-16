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

#include "actor/tab/ttab_attribute.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/log/nlog.h"
#include "net/tcp/ntcp.h"

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
		std::map<EnumAttribute, int64_t>	m_attr;		// [Absolute] attribute
		std::map<EnumAttribute, float>		m_rattr;	// Attribute[ ]
		std::map<EnumAttribute, float>		m_orattr;	// Attribute[ ]
		
		// ## Under attribute onadd attribute ##
		std::map<EnumAttribute, int64_t>	m_fight;						// [M_attr+m_rattr] generate attribute
		int64_t								m_fightscore = 0;				// Translated comment.
		EnumModule							m_module = EnumModule::E_ModuleNull;
		std::map<EnumModule, std::map<EnumAttribute, float>> m_crattr;		// Module attribute, [ attribute]onadd
	private:
		void update(std::map<EnumAttribute, int64_t>& aattr, const std::map<EnumAttribute, float>& amr);

		int64_t fight();
	public:
		attribute_value() = default;

		std::map<EnumAttribute, int64_t>& get_fight();

		int64_t update();

		// Attribute
		void printf();

		// Clearattribute
		void clear();

		// Addattribute
		void set_attr(EnumAttribute atype, double avalues);

		// Getattribute
		double get_attr(EnumAttribute atype);

		// Add attributeattribute
		void set_rattr(EnumAttribute atype, double avalues);

		// Get attributeattribute
		double get_rattr(EnumAttribute atype);

		// To nodeaddattribute
		void set_father_rattr(EnumModule amodule, EnumAttribute atype, double avalues);

		// Get node attribute
		double get_father_rattr(EnumModule amodule, EnumAttribute atype);

		// Translated comment.
		int64_t fightscore();

		// Convert pbstructure
		void topb(pbdb::UNIT_MODULE& aunitmodule);
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
		auto lpair = ngl::enum_attr_name::name(aval);
		return std::format_to(ctx.out(), "{}:{}", lpair.first, lpair.second);
	}
};