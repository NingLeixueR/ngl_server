#pragma once


namespace ngl
{
	enum ELOG
	{
		ELOG_DEBUG,   // 测试信息
		ELOG_INFO,    // 打印
		ELOG_WARN,    // 警告
		ELOG_ERROR,   // 错误
		ELOG_NONE,    // 无
	};

	struct sysconfig
	{
	private:
		//##### 日志相关
		static ELOG		m_loglevel;			// 日志等级
		static int32_t	m_logline;			// 单个日志文件的行数
		static int32_t	m_logflushtime;		// 日志flush时间
		static bool		m_logiswrite;		// 日志是否写入文件
		static bool		m_logconsole;		// 是否打印到控制台

		static int32_t	m_consumings;		// 检测actor消息耗时

		// 设置简单加密
		static std::string	m_xorkey;
		static int32_t		m_xorkeynum;	//2^n-1 n为xorkey的字符数
		static bool			m_isxor;
		
		static bool			m_varint;		// 是否开启varint

		static bool			m_robot_test;	// 是否机器人测试
	public:
		static void init();

		static ELOG loglevel()
		{ 
			return m_loglevel;
		}
	
		static int32_t logline()
		{
			return m_logline;
		}

		static int32_t logflushtime()
		{
			return m_logflushtime;
		}

		static bool logiswrite()
		{
			return m_logiswrite;
		}

		static bool logconsole()
		{
			return m_logconsole;
		}

		static int32_t consumings()
		{
			return m_consumings;
		}

		static std::string& xorkey()
		{
			return m_xorkey;
		}

		static int32_t xorkeynum()
		{
			return m_xorkeynum;
		}

		static bool isxor()
		{
			return m_isxor;
		}

		static bool varint()
		{
			return m_varint;
		}

		static bool robot_test()
		{
			return m_robot_test;
		}
	};
}//namespace ngl