/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
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

#if defined(WIN32)||defined(WINCE)||defined(WIN64)
# define Throw(FORMAT,...) throw std::runtime_error(std::format(FORMAT, ##__VA_ARGS__))
#else
# define Throw(FORMAT,...) throw std::runtime_error(std::format(FORMAT __VA_OPT__(,) ##__VA_ARGS__))
#endif

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
