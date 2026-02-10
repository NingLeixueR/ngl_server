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
	struct ttab_dbload :
		public csv<tab_dbload>
	{
		ttab_dbload(const ttab_dbload&) = delete;
		ttab_dbload& operator=(const ttab_dbload&) = delete;

		std::map<std::string, tab_dbload*> m_name2data;

		void reload()final
		{
			std::cout << "[ttab_dbload] reload" << std::endl;
			foreach([&](tab_dbload& atab)
				{
					std::string lname = atab.m_name;
					tools::transform_tolower(lname);
					m_name2data[lname] = &atab;
				}
			);
		}
	public:
		using type_tab = tab_dbload;

		ttab_dbload() = default;

		static ttab_dbload& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_dbload>();
			}
			return *ncsv::get<ttab_dbload>();
		}

		// # std::map<int, tab_dbload>& tabs()
		// # tab_dbload* tab(int aid)

		template <typename T>
		tab_dbload* get_tabdb()
		{
			tab_dbload** tab = tools::findmap(m_name2data, tools::type_name<T>());
			if (tab == nullptr)
			{
				return nullptr;
			}
			return *tab;
		}
	};
}//namespace ngl
