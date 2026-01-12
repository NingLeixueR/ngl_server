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

#include "localtime.h"
#include "logprintf.h"
#include "sysconfig.h"
#include "tools.h"

#include <source_location>
#include <sstream>
#include <format>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	class elog_name
	{
	public:
		static const char* get(ELOGLEVEL atype);
	};

	struct nactor_logitem;

	extern std::shared_ptr<nactor_logitem> log_debug(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<nactor_logitem> log_debug_net(const std::source_location& asource = std::source_location::current());

	extern std::shared_ptr<nactor_logitem> log_info(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<nactor_logitem> log_info_net(const std::source_location& asource = std::source_location::current());

	extern std::shared_ptr<nactor_logitem> log_warn(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<nactor_logitem> log_warn_net(const std::source_location& asource = std::source_location::current());

	extern std::shared_ptr<nactor_logitem> log_error(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<nactor_logitem> log_error_net(const std::source_location& asource = std::source_location::current());

	extern std::shared_ptr<nactor_logitem> log_bi(const std::source_location& asource = std::source_location::current());


	template <typename T>
	void tools::print_json(const T& adata, bool aislog/* = false*/)
	{
		std::string ljson;
		if (tools::proto2json(adata, ljson))
		{
			log_error()->print("{}:{}", tools::type_name<T>(), tools::format_json(ljson));
		}
	}
}//namespace ngl

#define Assert(ISOK)	((ISOK)== false)?(throw #ISOK,0):1

#define Try				try
#define Catch																					\
	catch(const char* errormsg)																	\
	{																							\
		ngl::log_error()->print("function[{}][{}]", __FUNCTION__, errormsg);					\
	}																							\
	catch(const std::exception& e)																\
	{																							\
		ngl::log_error()->print("function[{}][{}]", __FUNCTION__, e.what());					\
	}																							\
	catch(...)																					\
	{																							\
		ngl::log_error()->print("function[{}]", __FUNCTION__);									\
	}

template <typename T>
struct std::formatter<std::vector<T>>
{
	constexpr auto parse(const std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::vector<T>& vec, std::format_context& ctx)const
	{
		auto out = ctx.out();
		std::format_to(out, "vector[");
		for (auto it = vec.begin(); it != vec.end(); ++it) 
		{
			std::format_to(out, "{},", *it);
		}
		format_to(out, "]");
		return out;
	}
};

template <typename T>
struct std::formatter<std::list<T>>
{
	constexpr auto parse(const std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::vector<T>& vec, std::format_context& ctx)const
	{
		auto out = ctx.out();
		std::format_to(out, "list[");
		for (auto it = vec.begin(); it != vec.end(); ++it) 
		{
			std::format_to(out, "{},", *it);
		}
		format_to(out, "]");
		return out;
	}
};

template <typename T>
struct std::formatter<std::set<T>>
{
	constexpr auto parse(const std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::set<T>& aset, std::format_context& ctx)const
	{
		auto out = ctx.out();
		std::format_to(out, "set[");
		for (auto it = aset.begin(); it != aset.end(); ++it)
		{
			std::format_to(out, "{},", *it);
		}
		format_to(out, "]");
		return out;
	}
};

template <typename TKEY, typename TVAL>
struct std::formatter<std::map<TKEY, TVAL>>
{
	constexpr auto parse(const std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::map<TKEY, TVAL>& vec, std::format_context& ctx)const
	{
		auto out = ctx.out();
		std::format_to(out, "map[");
		for (auto it = vec.begin(); it != vec.end(); ++it) 
		{
			std::format_to(out, "({}:{}),", it->first, it->second);
		}
		format_to(out, "]");
		return out;
	}
};

template <typename T>
struct std::formatter<google::protobuf::RepeatedField<T>>
{
	constexpr auto parse(const std::format_parse_context& ctx)const
	{
		return ctx.begin();
	}

	auto format(const google::protobuf::RepeatedField<T>& aval, std::format_context& ctx)const
	{
		auto out = ctx.out();
		std::format_to(out, "protobuf::RepeatedField[");
		for(int i = 0;i< aval.size();++i)
		{
			if (i + 1 == aval.size())
			{
				std::format_to(out, "{}", aval[i]);
			}
			else
			{
				std::format_to(out, "{},", aval[i]);
			}
		}
		format_to(out, "]");
		return out;
	}
};

#define mk_formatter(NAME)															\
template <>																			\
struct std::formatter<ngl::data_modified<NAME>>										\
{																					\
	constexpr auto parse(const std::format_parse_context& ctx)const					\
	{																				\
		return ctx.begin();															\
	}																				\
	auto format(const ngl::data_modified<NAME>& aval, std::format_context& ctx)const\
	{																				\
		std::string ldb;															\
		data_modified_return_getconst(lpdbdataconst, aval, ctx.out())				\
		ngl::tools::proto2json(*lpdbdataconst, ldb);								\
		return std::format_to(ctx.out(), #NAME":<{}>\n", ldb);						\
	}																				\
};
