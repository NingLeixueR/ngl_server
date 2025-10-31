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
			std::string		m_dir;					// �ļ���
			ELOG_TYPE		m_type = ELOG_DEFAULT;	// ��־����
			int32_t			m_flush_time = 0;	// д���ļ��ļ��

			int32_t flush_time()
			{
				return std::max(m_flush_time, (int32_t)min_flush_time);
			}
		};
		std::ofstream	m_stream;
		config			m_config;
		int				m_count = 0;
		int				m_fcount = 0;

		logfile(const config& aconfig);

		bool check_count()const;

		void close_fstream();

		bool create_directories(const std::string& apath);

		void create();

		virtual void printf(const np_logitem* alog) = 0;

		void flush();

		static std::shared_ptr<logfile> create_make(const config& aconfig);
	};
}// namespace ngl