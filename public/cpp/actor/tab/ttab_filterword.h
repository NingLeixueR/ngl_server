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

#include "nfilterword.h"
#include "ncsv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_filterword :
		public csv<tab_filterword>
	{
		ttab_filterword(const ttab_filterword&) = delete;
		ttab_filterword& operator=(const ttab_filterword&) = delete;

		void reload()final
		{
			std::cout << "[ttab_filterword] reload" << std::endl;
			nfilterword<std::string>::instance().clear();
			nfilterword<std::string>::instance().init();
			foreach([](tab_filterword& atab)
				{
					nfilterword<std::string>::instance().load(atab.m_content);
				}
			);
			nfilterword<std::string>::instance().build();
		}
	public:
		using type_tab = tab_filterword;

		ttab_filterword() = default;

		static ttab_filterword& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_filterword>();
			}
			return *ncsv::get<ttab_filterword>();
		}

		// # std::map<int, tab_filterword>& tabs()
		// # tab_filterword* tab(int aid)
	};
}//namespace ngl
