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

		ttab_familylv()
		{}

		virtual void reload()
		{
			m_failylvexp.clear();
			m_failyrolecount.clear();
			for (const std::pair<const int, tab_familylv>& pair : tablecsv)
			{
				m_failylvexp[pair.first] = pair.second.m_exp;
				m_failyrolecount[pair.first] = pair.second.m_maxmembers;
			}
		}

		// ����lv��ȡ����Ҫ�ľ���
		static int32_t* failylvexp(int32_t alv)
		{
			return tools::findmap(m_failylvexp, alv);
		}

		// ����lv��ȡ���������
		static int32_t* failylvmaxcount(int32_t alv)
		{
			return tools::findmap(m_failyrolecount, alv);
		}
	};
}//namespace ngl