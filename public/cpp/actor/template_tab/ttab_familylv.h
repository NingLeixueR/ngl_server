#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_familylv :
		public manage_csv<tab_familylv>
	{
		ttab_familylv(const ttab_familylv&) = delete;
		ttab_familylv& operator=(const ttab_familylv&) = delete;

		std::map<int32_t, int32_t> m_failylvexp;		// key:lv value:exp
		std::map<int32_t, int32_t> m_failyrolecount;	// key:lv value:rolecount

		ttab_familylv()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_familylv] reload" << std::endl;
			m_failylvexp.clear();
			m_failyrolecount.clear();
			for (const auto& apair : tablecsv())
			{
				m_failylvexp[apair.first] = apair.second.m_exp;
				m_failyrolecount[apair.first] = apair.second.m_maxmembers;
			}
		}
	public:
		using type_tab = tab_familylv;

		static ttab_familylv& instance()
		{
			static ttab_familylv ltemp;
			return ltemp;
		}

		const std::map<int, tab_familylv>& tablecsv()
		{
			ttab_familylv* ttab = allcsv::get<ttab_familylv>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_familylv* tab(int32_t aid)
		{
			auto itor = tablecsv().find(aid);
			if (itor == tablecsv().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

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
