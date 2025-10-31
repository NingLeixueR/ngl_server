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
#include "operator_file.h"
#include "nprotocol.h"
#include "tools.h"
#include "nlog.h"
#include "csv.h"

namespace ngl
{
	bool rcsv::read(const std::string& aname, std::string& averify)
	{
		//# 读取文件
		readfile lrf(aname);
		//# 跳过前3行表头
		lrf.jumpbegin(3, true);

		if (lrf.readcurrent(m_data) == false)
		{
			std::cout << std::format("loadcsv fail #{}", aname) << std::endl;
			return false;
		}
		averify = tools::md5(m_data);
		std::cout << std::format("loadcsv#{}#{}", aname, averify) << std::endl;
		return true;
	}
}// namespace ngl