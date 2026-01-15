#include "bi_item.h"
#include "xmlprotocol.h"

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
			static std::array<const char*, std::tuple_size<decltype(std::make_tuple())>::value>& parms() 
			{
				static std::array<const char*, std::tuple_size<decltype(std::make_tuple())>::value> tempvec; 
				static std::string tempstr(""); static std::atomic lregister = true; 
				if (lregister.exchange(false) && !tempstr.empty()) 
				{
					ngl::tools::split_str<std::tuple_size<decltype(std::make_tuple())>::value>(&tempstr[0], (int32_t)tempstr.size(), tempvec); 
					if constexpr (true) 
					{
						for (const char*& item : tempvec) 
						{
							if (memcmp("m_", item, 2) == 0) 
							{
								item = &(item[2]);
							}
						}
					}
				}
				return tempvec;
			} 
			inline bool xml_pop(const char* axml) 
			{
				tinyxml2::XMLDocument ldocument; 
				ngl::xml::readxml(axml, ldocument); 
				tinyxml2::XMLElement* lelement = ngl::xml::get_child(ldocument, "co"); 
				return xml_pop(lelement);
			} 
			inline bool xml_push(const char* axml)const 
			{
				tinyxml2::XMLDocument ldocument; 
				tinyxml2::XMLElement* lelement = xml::set_child(ldocument, "co"); 
				if (!xml_push(lelement)) 
				{
					return false;
				} 
				return xml::writexml(axml, ldocument);
			} 
			inline bool xml_pop(tinyxml2::XMLElement* aele) 
			{
				return ngl::xserialize<false>::pop(aele, parms());
			} 
			inline bool xml_push(tinyxml2::XMLElement* aele)const 
			{
				return ngl::xserialize<false>::push(aele, parms());
			}
		};
		{
			test_xml ltemp;
			bool lbool = ltemp.xml_push("1.xml");
		}
		{
			test_xml ltemp;
			bool lbool = ltemp.xml_pop("1.xml");
		}
	}

}