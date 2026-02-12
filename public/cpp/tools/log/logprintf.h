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

#include <fstream>
#include <memory>

#include "sysconfig.h"

namespace ngl
{
	enum ELOG_TYPE
	{
		ELOG_DEFAULT	= 0x00000001,
		ELOG_BI			= 0x00000002,
		ELOG_COUNT,
	};

	class logprintf
	{
	public:
		static void printf(ELOGLEVEL acolor, const char* apos, const char* atimestr, const char* amsg);
	};

	struct np_logitem;

	struct logfile
	{
		struct config
		{
			enum
			{
				min_flush_time = 1000,
			};

			i32_actordataid m_id;					// nlogactor
			std::string		m_dir;					// 文件夹
			ELOG_TYPE		m_type = ELOG_DEFAULT;	// 日志类型
			int32_t			m_flush_time = 0;		// 写入文件的间隔

			int32_t flush()
			{
				return std::max(m_flush_time, (int32_t)min_flush_time);
			}
		};
		std::ofstream	m_stream;
		config			m_config;		// 日志配置
		int				m_count = 0;	// 数量
		int				m_fcount = 0;	// 刷新数量

		logfile(const config& aconfig);

		// # 刷新数量
		bool flush_count()const;

		// # 关闭文件流
		void close_fstream();

		// # 创建目录
		bool create_dir(const std::string& apath);

		// # 创建
		void create();

		// # 打印日志
		virtual void printf(const np_logitem* alog) = 0;

		// # 刷新到文件
		void flush();

		// # 创建日志实例
		static std::shared_ptr<logfile> create_make(const config& aconfig);
	};
}// namespace ngl