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

#include "ncsv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_errormessage :
		public csv<tab_errormessage>
	{
		ttab_errormessage(const ttab_errormessage&) = delete;
		ttab_errormessage& operator=(const ttab_errormessage&) = delete;

		void reload()final
		{
			std::cout << "[ttab_errormessage] reload" << std::endl;
		}
	public:
		using type_tab = tab_errormessage;

		ttab_errormessage() = default;

		static ttab_errormessage& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_errormessage>();
			}
			return *ncsv::get<ttab_errormessage>();
		}

		// # std::map<int, tab_errormessage>& tabs()
		// # tab_errormessage* tab(int aid)
	};
}//namespace ngl
