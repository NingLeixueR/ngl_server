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
	struct ttab_familylv :
		public csv<tab_familylv>
	{
		ttab_familylv(const ttab_familylv&) = delete;
		ttab_familylv& operator=(const ttab_familylv&) = delete;

		std::map<int32_t, int32_t> m_failylvexp;		// key:lv value:exp
		std::map<int32_t, int32_t> m_failyrolecount;	// key:lv value:rolecount

		void reload()final
		{
			std::cout << "[ttab_familylv] reload" << std::endl;
			m_failylvexp.clear();
			m_failyrolecount.clear();

			foreach([&](tab_familylv& atab)
				{
					m_failylvexp[atab.m_id] = atab.m_exp;
					m_failyrolecount[atab.m_id] = atab.m_maxmembers;
				}
			);
		}
	public:
		using type_tab = tab_familylv;

		ttab_familylv() = default;

		static ttab_familylv& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_familylv>();
			}
			return *ncsv::get<ttab_familylv>();
		}

		// # std::map<int, tab_familylv>& tabs()
		// # tab_familylv* tab(int aid)

		// 根据lv获取其需要的经验
		int32_t* failylvexp(int32_t alv)
		{
			return tools::findmap(m_failylvexp, alv);
		}

		// 根据lv获取其最大人数
		int32_t* failylvmaxcount(int32_t alv)
		{
			return tools::findmap(m_failyrolecount, alv);
		}
	};
}//namespace ngl
