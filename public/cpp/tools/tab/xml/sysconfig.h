#pragma once

#include "type.h"
#include <vector>

namespace ngl
{
	enum ELOGLEVEL
	{
		ELOG_NONE = -1,		// 无
		ELOG_DEBUG = 0,		// 测试信息
		ELOG_INFO,			// 打印
		ELOG_WARN,			// 警告
		ELOG_ERROR,			// 错误
	};

	struct sysconfig
	{
	private:
		//##### 日志相关
		static ELOGLEVEL	m_loglevel;				// 日志等级
		static int32_t		m_logline;				// 单个日志文件的行数
		static int32_t		m_logflushtime;			// 日志flush时间
		static bool			m_logiswrite;			// 日志是否写入文件
		static bool			m_logconsole;			// 是否打印到控制台

		static int32_t		m_consumings;			// 检测actor消息耗时

		// 设置简单加密
		static std::string	m_xorkey;
		static int32_t		m_xorkeynum;			//2^n-1 n为xorkey的字符数
		static bool			m_isxor;
		
		static bool			m_varint;				// 是否开启varint

		static bool			m_robot_test;			// 是否机器人测试

		static int32_t		m_kcpping;				// kcp发送ping的间隔(毫秒)
		static int32_t		m_kcppinginterval;		// kcp检查ping的间隔(毫秒)
		static std::string	m_kcpsession;			// kcp session

		static int32_t		m_sessionewait;			// 等待gateway移除session的时间(毫秒)
		static int32_t		m_open_servertime;		// 开服时间

		static int32_t		m_head_version;			// 协议头的版本号

		static int32_t		m_rate_interval;		// 速率检测的间隔(单位秒)
		static int32_t		m_rate_count;			// 速率 每秒多少条
		static int32_t		m_heart_beat_interval;	// 心跳间隔(单位秒)
		static int32_t		m_net_timeout;			// 用来检查包头中的时间与服务器时间

		static std::string	m_gmurl;				// gm url

		// # 网关列表
		static std::vector<i32_serverid> m_gatewayids;
	public:
		static void init();

		static ELOGLEVEL loglevel()		{ return m_loglevel; }
		static int32_t logline()		{ return m_logline; }
		static int32_t logflushtime()	{ return m_logflushtime; }
		static bool logiswrite()		{ return m_logiswrite; }
		static bool logconsole()		{ return m_logconsole; }
		static int32_t consumings()		{ return m_consumings; }
		static std::string& xorkey()	{ return m_xorkey; }
		static int32_t xorkeynum()		{ return m_xorkeynum; }
		static bool isxor()				{ return m_isxor; }
		static bool varint()			{ return m_varint; }
		static bool robot_test()		{ return m_robot_test; }
		static int32_t kcpping()		{ return m_kcpping; }
		static int32_t kcppinginterval(){ return m_kcppinginterval; }
		static std::string& kcpsession(){ return m_kcpsession; }
		static int32_t sessionwait()	{ return m_sessionewait; }
		static int32_t open_servertime(){ return m_open_servertime; }
		static int32_t head_version()	{ return m_head_version; }
		static int32_t rate_interval()	{ return m_rate_interval; }		
		static int32_t rate_count()		{ return m_rate_count; }
		static int32_t heart_beat_interval() { return m_heart_beat_interval; }
		static int32_t net_timeout()	{ return m_net_timeout; }
		static std::string& gmurl()		{ return m_gmurl; }
		
		static void init_gatewayids();
		static std::vector<i32_serverid>& gatewayids();
	};
}//namespace ngl