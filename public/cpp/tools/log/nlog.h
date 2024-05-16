#pragma once

#include "logformat.h"
#include "localtime.h"
#include "logprintf.h"
#include "sysconfig.h"

#include <format>
#include <sstream>
#include <string>
#include <vector>
#include <map>


namespace ngl
{
	class elog_name
	{
	public:
		static const char* get(ELOGLEVEL atype);
	};
	struct np_actor_logitem;
	extern std::shared_ptr<np_actor_logitem> log(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<np_actor_logitem> lognet(const std::source_location& asource = std::source_location::current());

}

#define Assert(ISOK)	\
		if(ISOK)		\
		{				\
		}				\
		else			\
		{				\
			throw #ISOK;\
		}

#define Try				try
#define Catch																					\
	catch(const char* errormsg)																	\
	{																							\
		ngl::log()->error("function[{}][{}]", __FUNCTION__, errormsg);							\
	}																							\
	catch(const std::exception& e)																\
	{																							\
		ngl::log()->error("function[{}][{}]", __FUNCTION__, e.what());							\
	}																							\
	catch(...)																					\
	{																							\
		ngl::log()->error("function[{}]", __FUNCTION__);										\
	}




template <typename T>
struct std::formatter<std::vector<T>>
{
	auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::vector<T>& vec, std::format_context& ctx)
	{
		auto out = ctx.out();
		std::format_to(out, "[");
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
	auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::vector<T>& vec, std::format_context& ctx)
	{
		auto out = ctx.out();
		std::format_to(out, "[");
		for (auto it = vec.begin(); it != vec.end(); ++it) 
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
	auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::map<TKEY, TVAL>& vec, std::format_context& ctx)
	{
		auto out = ctx.out();
		std::format_to(out, "[");
		for (auto it = vec.begin(); it != vec.end(); ++it) 
		{
			std::format_to(out, "({}:{}),", it->first, it->second);
		}
		format_to(out, "]");
		return out;
	}
};