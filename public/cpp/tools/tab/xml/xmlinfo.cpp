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
#include "nprotocol.h"
#include "xmlinfo.h"
#include "tools.h"
#include "nlog.h"

#include <string>

namespace ngl
{
	bool xarg_info::find(const char* akey, bool& adata)
	{
		std::string* lp = tools::findmap(m_data, akey);
		if (lp == nullptr)
		{
			return false;
		}
		adata = *lp == "true" || *lp == "TRUE";
		return true;
	}

	bool xarg_info::find(const char* akey, std::string& adata)
	{
		std::string* lp = tools::findmap(m_data, akey);
		if (lp == nullptr)
		{
			return false;
		}
		adata = *lp;
		return true;
	}

	void xarg_info::foreach(const std::function<void(const std::pair<const std::string, std::string>&)>& afun)
	{
		for (const auto& item : m_data)
		{
			afun(item);
		}
	}
}// namespace ngl