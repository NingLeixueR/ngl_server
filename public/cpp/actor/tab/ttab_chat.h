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
	struct ttab_chat :
		public csv<tab_chat>
	{
		ttab_chat(const ttab_chat&) = delete;
		ttab_chat& operator=(const ttab_chat&) = delete;

		void reload()final
		{
			std::cout << "[ttab_chat] reload" << std::endl;
		}
	public:
		using type_tab = tab_chat;

		ttab_chat() = default;

		static ttab_chat& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_chat>();
			}
			return *ncsv::get<ttab_chat>();
		}

		// # std::map<int, tab_chat>& tabs()
		// # tab_chat* tab(int aid)
	};
}//namespace ngl
