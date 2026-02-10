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
	struct ttab_activity_drawcompliance :
		public csv<tab_activity_drawcompliance>
	{
		ttab_activity_drawcompliance(const ttab_activity_drawcompliance&) = delete;
		ttab_activity_drawcompliance& operator=(const ttab_activity_drawcompliance&) = delete;

		void reload()final
		{
			std::cout << "[ttab_activity_drawcompliance] reload" << std::endl;
		}
	public:
		using type_tab = tab_activity_drawcompliance;

		ttab_activity_drawcompliance() = default;

		static ttab_activity_drawcompliance& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_activity_drawcompliance>();
			}
			return *ncsv::get<ttab_activity_drawcompliance>();
		}

		// # std::map<int, tab_activity_drawcompliance>& tabs()
		// # tab_activity_drawcompliance* tab(int aid)
	};
}//namespace ngl