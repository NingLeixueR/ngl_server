#include "ttab_random.h"
#include "nprotocol.h"

namespace ngl
{
	void ttab_random::reload()
	{
		// ## ��������ӵ����Ƿ�ѭ������
		for (std::pair<const int, tab_random>& ipair : m_tablecsv)
		{
			tab_random& tab = ipair.second;
			std::set<int32_t> lset;
			if (is_loop(tab.m_id, lset) == false)
			{
				log_error()->print("ttab_random::reload random[{}] loop!!!", tab.m_id);
			}
		}
	}
}