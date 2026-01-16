#include "xmlprotocol.h"
#include "manage_curl.h"
#include "bi_item.h"
#include "dbredis.h"

#include <utf8.h> 

namespace ngl
{
	void server_test()
	{
		{
			ngl::bi_item ltemp;
			ltemp.m_serverid = 1;
			ltemp.m_roleid = 2;
			ltemp.m_itemid = 3;
			ltemp.m_tid = 4;
			ltemp.m_count = 5;
			ltemp.m_time = 6;
			ltemp.m_adddec = 7;// 1:‘ˆº” 2:ºı…Ÿ
			ltemp.m_mask = "mask";
			ltemp.print();
		}
		

		{
			ngl::xarg_protocols ltemp;
			ltemp.m_config.push_back(
				ngl::xarg_protocols::info
				{
					.m_client = 1,
					.m_name = "k1",
					.m_number = 1
				}
			);
			ltemp.m_config.push_back(
				ngl::xarg_protocols::info
				{
					.m_client = 2,
					.m_name = "k1",
					.m_number = 2
				}
			);
			ltemp.m_config.push_back(
				ngl::xarg_protocols::info
				{
					.m_client = 3,
					.m_name = "k1",
					.m_number = 3
				}
			);
			ltemp.xml_push("19890519.xml");
		}


		struct test_xml
		{
			dxmlserialize(test_xml, false)
		};
		{
			test_xml ltemp;
			bool lbool = ltemp.xml_push("1.xml");
		}
		{
			test_xml ltemp;
			bool lbool = ltemp.xml_pop("1.xml");
		}

		//test_hiredis();
	}
}