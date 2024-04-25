#pragma once

#include "logformat.h"
#include "localtime.h"
#include "logprintf.h"

#include <string>
#include <map>

namespace ngl
{
	class elog_name
	{
	public:
		static const char* get(ELOG atype);
	};

	class nlog
	{
		nlog(){}
		bool m_isinitfinish = false;
	public:
		static nlog& getInstance()
		{
			static nlog ltemp;
			return ltemp;
		}

		bool& isinitfinish();

		void plog(ELOG atype, ngl::logformat& llogformat, ELOG_TYPE altype);
	};
}

#define LogFomat(llogformat, NAME, FORMAT,...)  llogformat.out(NAME, FORMAT,##__VA_ARGS__)	

#ifdef WIN32
# define FindSrcPos str.rfind("\\")
#else
# define FindSrcPos str.rfind("/")
#endif

#define LogSrcPos														\
	constexpr std::string_view str = __FILE__;							\
	constexpr auto pos = FindSrcPos;									\
	if constexpr (pos != std::string_view::npos)						\
	{																	\
		constexpr std::string_view str2 = str.substr(pos + 1);			\
		llogformat.format("pos", "(%:%)",str2.data(), __LINE__);		\
	}

#define DEF_LOG_LEVEL			(ngl::ELOG_WARN)
#define DEF_LOG_MAX_LINE		(100000)
#define DEF_LOG_PRINTF			(true)

#if defined(WIN32)||defined(WINCE)||defined(WIN64)
# define dlogmsg(ELOG_LEVEL, TYPE, FORMAT,...)										\
	{																					\
		if(DEF_LOG_LEVEL <= ELOG_LEVEL)													\
		{																				\
			ngl::logformat llogformat;													\
			LogSrcPos																	\
			llogformat.data("head") = ngl::localtime::time2msstr("%Y-%m-%d %H:%M:%S");	\
			llogformat.format("src", FORMAT,##__VA_ARGS__);								\
			ngl::nlog::getInstance().plog(ELOG_LEVEL, llogformat, TYPE);				\
		}																				\
	}
# define LogDebug(FORMAT,...)			dlogmsg(ngl::ELOG_DEBUG, ngl::ELOG_NETWORK,	FORMAT,##__VA_ARGS__)
# define LogError(FORMAT,...)			dlogmsg(ngl::ELOG_ERROR, ngl::ELOG_NETWORK,	FORMAT,##__VA_ARGS__)
# define LogInfo(FORMAT,...)			dlogmsg(ngl::ELOG_INFO,	 ngl::ELOG_NETWORK,	FORMAT, ##__VA_ARGS__)
# define LogWarn(FORMAT,...)			dlogmsg(ngl::ELOG_WARN,  ngl::ELOG_NETWORK,	FORMAT,##__VA_ARGS__)
#define LogLocalDebug(FORMAT,...)		dlogmsg(ngl::ELOG_DEBUG, ngl::ELOG_LOCAL, FORMAT,##__VA_ARGS__)
#define LogLocalError(FORMAT,...)		dlogmsg(ngl::ELOG_ERROR, ngl::ELOG_LOCAL, FORMAT,##__VA_ARGS__)
#define LogLocalInfo(FORMAT,...)		dlogmsg(ngl::ELOG_INFO,  ngl::ELOG_LOCAL,	FORMAT,##__VA_ARGS__)
#define LogLocalWarn(FORMAT,...)		dlogmsg(ngl::ELOG_WARN,  ngl::ELOG_LOCAL, FORMAT,##__VA_ARGS__)
#define LogBI(FORMAT,...)				dlogmsg(ngl::ELOG_ERROR,  ngl::ELOG_BI, FORMAT,##__VA_ARGS__)
#else
# define dlogmsg(ELOG_LEVEL, TYPE, FORMAT,...)										\
	{																					\
		if(DEF_LOG_LEVEL <= ELOG_LEVEL)													\
		{																				\
			ngl::logformat llogformat;													\
			LogSrcPos																	\
			llogformat.data("head") = ngl::localtime::time2msstr("%Y-%m-%d %H:%M:%S");	\
			llogformat.format("src", FORMAT __VA_OPT__(,) ##__VA_ARGS__);				\
			ngl::nlog::getInstance().plog(ELOG_LEVEL, llogformat, TYPE);				\
		}																				\
	}
# define LogDebug(FORMAT,...)			dlogmsg(ngl::ELOG_DEBUG, ngl::ELOG_NETWORK, FORMAT __VA_OPT__(,) ##__VA_ARGS__)
# define LogError(FORMAT,...)			dlogmsg(ngl::ELOG_ERROR, ngl::ELOG_NETWORK, FORMAT __VA_OPT__(,) ##__VA_ARGS__)
# define LogInfo(FORMAT,...)			dlogmsg(ngl::ELOG_INFO, ngl::ELOG_NETWORK,FORMAT __VA_OPT__(,)  ##__VA_ARGS__)
# define LogWarn(FORMAT,...)			dlogmsg(ngl::ELOG_WARN, ngl::ELOG_NETWORK, FORMAT __VA_OPT__(,) ##__VA_ARGS__)
#define LogLocalDebug(FORMAT,...)		dlogmsg(ngl::ELOG_DEBUG, ngl::ELOG_LOCAL, FORMAT __VA_OPT__(,) ##__VA_ARGS__)
#define LogLocalError(FORMAT,...)		dlogmsg(ngl::ELOG_ERROR, ngl::ELOG_LOCAL, FORMAT  __VA_OPT__(,) ##__VA_ARGS__)
#define LogLocalInfo(FORMAT,...)		dlogmsg(ngl::ELOG_INFO, ngl::ELOG_LOCAL,FORMAT __VA_OPT__(,) ##__VA_ARGS__)
#define LogLocalWarn(FORMAT,...)		dlogmsg(ngl::ELOG_WARN, ngl::ELOG_LOCAL, FORMAT  __VA_OPT__(,) ##__VA_ARGS__)
#define LogBI(FORMAT,...)				dlogmsg(ngl::ELOG_WARN, ngl::ELOG_BI, FORMAT  __VA_OPT__(,) ##__VA_ARGS__)
#endif

#define Try				try
#define Catch																					\
	catch(const char* errormsg)																	\
	{																							\
		LogLocalError("function[%][%]",__FUNCTION__,errormsg)									\
	}																							\
	catch(const std::exception& e)																\
	{																							\
		LogLocalError("function[%][%]",__FUNCTION__,e.what())									\
	}																							\
	catch(...)																					\
	{																							\
		LogLocalError("function[%]",__FUNCTION__)												\
	}

#define Assert(ISOK)	\
		if(ISOK)		\
		{				\
		}				\
		else			\
		{				\
			throw #ISOK;\
		}

#define AssertFormat(ISOK,FORMAT,...)		\
		if(ISOK)							\
		{									\
		}									\
		else								\
		{									\
			ngl::logformat::out([](char* abuff) {throw abuff; }, #ISOK":"##FORMAT, ##__VA_ARGS__); \
		}