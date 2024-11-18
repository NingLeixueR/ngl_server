#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{

	struct ttab_familylv : public manage_csv<tab_familylv>
	{
		ttab_familylv(const ttab_familylv&) = delete;
		ttab_familylv& operator=(const ttab_familylv&) = delete;
	private:
		static std::map<int32_t, int32_t> m_failylvexp;		// key:lv value:exp
		static std::map<int32_t, int32_t> m_failyrolecount;	// key:lv value:rolecount
	public:

		ttab_familylv() = default;

		void reload()final
		{
			m_failylvexp.clear();
			m_failyrolecount.clear();
			std::ranges::for_each(m_tablecsv, [this](const auto& apair)
				{
					m_failylvexp[apair.first] = apair.second.m_exp;
					m_failyrolecount[apair.first] = apair.second.m_maxmembers;
				});
		}

		// 根据lv获取其需要的经验
		static int32_t* failylvexp(int32_t alv)
		{
			return tools::findmap(m_failylvexp, alv);
		}

		// 根据lv获取其最大人数
		static int32_t* failylvmaxcount(int32_t alv)
		{
			return tools::findmap(m_failyrolecount, alv);
		}
	};
}//namespace ngl
