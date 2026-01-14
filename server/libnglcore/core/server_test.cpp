#include "bi_item.h"

namespace ngl
{
	void server_test()
	{
		ngl::bi_item ltemp;
		ltemp.m_serverid = 1;
		ltemp.m_roleid = 2;
		ltemp.m_itemid = 3;
		ltemp.m_tid = 4;
		ltemp.m_count = 5;
		ltemp.m_time = 6;
		ltemp.m_adddec = 7;// 1:Ôö¼Ó 2:¼õÉÙ
		ltemp.m_mask = "mask";
		ltemp.print();
	}

}