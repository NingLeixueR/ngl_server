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
	/**
		���ļ��ڶ����externȫ������ngl��log_����
		����actor���ڲ������actor_base�е�log_����
		log_���������ȫ�ֵ��û�д���Ӧsys_globalĿ¼�µ���־�ļ�
		log_�������actor���ڵ������д���Ӧactor_xxxĿ¼��
	*/

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
	void tools::print_json2proto(const T& adata, bool aislog/* = false*/)
	{
		std::string json;
		if (tools::proto2json(adata, json))
		{
			const google::protobuf::Descriptor* descriptor = adata.GetDescriptor();
			if (descriptor != nullptr)
			{
				if (aislog)
				{
					log_error()->print("{}:{}", descriptor->full_name(), json);
				}
				else
				{
					std::cout << std::format("{}:{}", descriptor->full_name(), json) << std::endl;
				}
			}
			else
			{
				if (aislog)
				{
					log_error()->print("{}", json);
				}
				else
				{
					std::cout << std::format("{}", json) << std::endl;
				}
			}
		}
	}
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
