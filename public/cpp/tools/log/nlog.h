#pragma once

#include "logformat.h"
#include "localtime.h"
#include "logprintf.h"

#include <sstream>
#include <string>

namespace ngl
{
	class elog_name
	{
	public:
		static const char* get(ELOG atype);
	};

	class nlog
	{
		nlog();
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
# define FindSrcPos(STR) STR.rfind("\\")
#else
# define FindSrcPos(STR) STR.rfind("/")
#endif

#define LogSrcPos														\
	constexpr std::string_view str = __FILE__;							\
	constexpr auto pos = FindSrcPos(str);								\
	if constexpr (pos != std::string_view::npos)						\
	{																	\
		constexpr std::string_view str2 = str.substr(pos + 1);			\
		llogformat.format("pos", "(%:%)",str2.data(), __LINE__);		\
	}

namespace ngl
{
	class nlogsys
	{
		static ELOG		m_level;		// 日志等级
		static int32_t	m_line;			// 单个日志文件的行数
		static int32_t	m_flushtime;	// 日志flush时间
		static bool		m_iswrite;		// 日志是否写入文件
		static bool		m_console;		// 是否打印到控制台
	public:
		static void init();

		static ELOG level()
		{
			return m_level;
		}

		static int32_t fline()
		{
			return m_line;
		}

		static int32_t flushtime()
		{
			return m_flushtime;
		}

		static bool iswrite()
		{
			return m_iswrite;
		}

		static bool console()
		{
			return m_console;
		}
	};
}



#if defined(WIN32)||defined(WINCE)||defined(WIN64)
# define dlogmsg(ELOG_LEVEL, TYPE, FORMAT,...)										\
	{																					\
		if(ngl::nlogsys::level() <= ELOG_LEVEL)													\
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
		if(ngl::nlogsys::level() <= ELOG_LEVEL)											\
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


namespace ngl
{
	class logstream
	{
		std::stringstream m_stream;
		ELOG m_log;
		ELOG_TYPE m_type;
	public:
		logstream(ELOG alog, ELOG_TYPE atype):
			m_log(alog),
			m_type(atype)
		{}

		template <typename T>
		logstream& operator<<(const T& adata)
		{
			m_stream << adata;
			return *this;
		}

		// 重载 << 操作符以输出 std::endl
		logstream& operator<<(std::ostream& (*manipulator)(std::ostream&)) 
		{
			m_stream << manipulator;
			return *this;
		}

		void print(const std::source_location& asource = std::source_location::current())
		{
			ngl::logformat llogformat;
			std::string_view str = asource.file_name();
			auto pos = FindSrcPos(str);
			if(pos != std::string_view::npos)
			{
				std::string_view str2 = str.substr(pos + 1);
				llogformat.format("pos", "(%:%)", str2.data(), asource.line());
			}			
			llogformat.data("head") = ngl::localtime::time2msstr("%Y-%m-%d %H:%M:%S");
			llogformat.format("src", "%", m_stream.str().c_str());
			ngl::nlog::getInstance().plog(m_log, llogformat, m_type);
		}
	};
}//namespace ngl

# define LogStreamDebug(STREAM)			ngl::logstream STREAM(ngl::ELOG_DEBUG, ngl::ELOG_NETWORK)
# define LogStreamError(STREAM)			ngl::logstream STREAM(ngl::ELOG_ERROR, ngl::ELOG_NETWORK)
# define LogStreamInfo(STREAM)			ngl::logstream STREAM(ngl::ELOG_INFO, ngl::ELOG_NETWORK)
# define LogStreamWarn(STREAM)			ngl::logstream STREAM(ngl::ELOG_WARN, ngl::ELOG_NETWORK)

# define LogLocalStreamDebug(STREAM)	ngl::logstream STREAM(ngl::ELOG_DEBUG, ngl::ELOG_LOCAL)
# define LogLocalStreamError(STREAM)	ngl::logstream STREAM(ngl::ELOG_ERROR, ngl::ELOG_LOCAL)
# define LogLocalStreamInfo(STREAM)		ngl::logstream STREAM(ngl::ELOG_INFO, ngl::ELOG_LOCAL)
# define LogLocalStreamWarn(STREAM)		ngl::logstream STREAM(ngl::ELOG_WARN, ngl::ELOG_LOCAL)