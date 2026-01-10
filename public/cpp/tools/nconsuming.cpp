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
#include "time_consuming.h"
#include "nprotocol.h"

namespace ngl
{
	nconsuming::nconsuming(const std::string& aname):
		m_name(aname),
		m_beg(0)
	{
	}

	void nconsuming::start()
	{
		m_beg = time_wheel::getms();
	}

	void nconsuming::finish()
	{
		int64_t lconsuming = time_wheel::getms() - m_beg;
		bool lerror = lconsuming > sysconfig::consumings();
		(lerror ? log_error() : log_info())->print("time consuming [{}:{}]", m_name, lconsuming);
	}
}//namespace ngl