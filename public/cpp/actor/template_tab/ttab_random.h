#pragma once

#include "manage_csv.h"
#include "type.h"
#include "nlog.h"
#include "xml.h"
#include "nlog.h"

namespace ngl
{
	class ttab_random :
		public manage_csv<tab_random>
	{
		ttab_random(const ttab_random&) = delete;
		ttab_random& operator=(const ttab_random&) = delete;

		ttab_random()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_random] reload" << std::endl;
			// ## 检查所有子掉落是否循环引用
			for (std::pair<const int, tab_random>& ipair : m_tablecsv)
			{
				tab_random& tab = ipair.second;
				std::set<int32_t> lset;
				if (is_loop(tab.m_id, lset));
				{
					tools::no_core_dump();
				}
			}
		}

	public:
		using type_tab = tab_random;

		static ttab_random& instance()
		{
			static ttab_random ltemp;
			return ltemp;
		}

		const std::map<int, tab_random>& tablecsv()
		{
			ttab_random* ttab = allcsv::get<ttab_random>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_random* tab(int32_t aid)
		{
			auto itor = tablecsv().find(aid);
			if (itor == tablecsv().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		bool is_loop(int32_t aid, std::set<int32_t>& aset)
		{
			const tab_random* ltab = tab(aid);
			if (ltab == nullptr)
			{
				return false;
			}
			if (aset.insert(ltab->m_id).second == false)
			{
				return false;
			}
			for (int32_t id : ltab->m_childrandomids)
			{
				if (is_loop(id, aset) == false)
				{
					return false;
				}
			}
			return true;
		}
	};
}//namespace ngl
