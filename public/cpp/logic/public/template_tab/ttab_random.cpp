#include "ttab_random.h"
#include "nprotocol.h"

namespace ngl
{
	void ttab_random::reload()
	{
		// ## 检查所有子掉落是否循环引用
		for (std::pair<const int, tab_random>& ipair : tablecsv)
		{
			tab_random& tab = ipair.second;
			std::set<int32_t> lset;
			if (is_loop(tab.m_id, lset) == false)
			{
				log()->error("ttab_random::reload random[{}] loop!!!", tab.m_id);
			}
		}
	}
}