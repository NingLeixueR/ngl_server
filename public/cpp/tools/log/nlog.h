#pragma once

#include "localtime.h"
#include "logprintf.h"
#include "sysconfig.h"

#include <source_location>
#include <sstream>
#include <format>
#include <string>
#include <vector>
#include <list>
#include <map>

namespace ngl
{
	class elog_name
	{
	public:
		static const char* get(ELOGLEVEL atype);
	};
	struct np_actor_logitem;
	extern std::shared_ptr<np_actor_logitem> log_debug(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<np_actor_logitem> log_debug_net(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<np_actor_logitem> log_info(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<np_actor_logitem> log_info_net(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<np_actor_logitem> log_warn(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<np_actor_logitem> log_warn_net(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<np_actor_logitem> log_error(const std::source_location& asource = std::source_location::current());
	extern std::shared_ptr<np_actor_logitem> log_error_net(const std::source_location& asource = std::source_location::current());
}//namespace ngl

#define Assert(ISOK)	 \
		if(ISOK)		 \
		{				 \
		}				 \
		else			 \
		{				 \
			throw #ISOK; \
		}

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
# define Throw(FORMAT,...) throw std::runtime_error(std::format(FORMAT, ##__VA_ARGS__));
#else
# define Throw(FORMAT,...) throw std::runtime_error(std::format(FORMAT __VA_OPT__(,) ##__VA_ARGS__));
#endif

template <typename T>
struct std::formatter<std::vector<T>>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::vector<T>& vec, std::format_context& ctx)const
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
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::vector<T>& vec, std::format_context& ctx)const
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
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::map<TKEY, TVAL>& vec, std::format_context& ctx)const
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
