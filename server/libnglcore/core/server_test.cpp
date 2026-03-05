#include "tools/tab/xml/xmlprotocol.h"
#include "tools/db/redis/dbredis.h"
#include "tools/log/bi/bi_item.h"
#include "tools/nfilterword.h"
#include "tools/curl/ncurl.h"

#include "tools/tab/json/njson.h"

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
			ltemp.m_adddec = 7;// 1:增加 2:减少
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
			DXMLSERIALIZE(test_xml, false)
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

		test_nfilterword();


		std::string ljsonstr = R"({
"title":"JSON Example",
"author": {
"name":"John Doe",
"age": 35,
"isVerified":true
},
"tags":["json", "syntax", "example"],
"rating": 4.5,
"isPublished":false
})";
		struct author
		{
			std::string name;
			int32_t age = 0;
			bool isVerified = false;

			bool json_pop(rapidjson::Value& ajson)
			{
				return ngl::njson::pop(ajson, { "name", "age", "isVerified"}, name, age, isVerified);
			}

			bool json_push(rapidjson::Value& ajson, rapidjson::Document::AllocatorType* aallocator)const
			{
				return ngl::njson::push(ajson, aallocator, { "name", "age", "isVerified" }, name, age, isVerified);
			}
		};
		{
			ncjson lnsjon2(ljsonstr.c_str());
			std::string title;
			std::vector<std::string> tags;
			float rating = 0.0f;
			bool isPublished = false;
			author lauthor;
			njson::pop(lnsjon2, { "title", "author", "tags", "rating", "isPublished" }
				, title
				, lauthor
				, tags
				, rating
				, isPublished
			);
		}

		std::string ljsonstrrr;
		{
			ncjson lnsjon21;
			std::string title = "china !!!";
			std::vector<std::string> tags = { "ribenrensb3", "meiguorensb3", "yingguorensb3" };
			float rating = 9.328f;
			bool isPublished = false;
			author lauthor;
			lauthor.name = "kangzhilong";
			lauthor.age = 38;
			lauthor.isVerified = true;

			std::vector<author> lvecauthors;
			for (int i = 0;i<10;++i)
			{
				author ltempauthor;
				ltempauthor.name = std::format("caozhen_{}", i);
				ltempauthor.age = i;
				ltempauthor.isVerified = i%2 == 1;
				lvecauthors.push_back(ltempauthor);
			}

			std::vector<int> lnumbers;
			for (int i = 0; i < 10; ++i)
			{
				lnumbers.push_back(i+1000);
			}

			std::map<int32_t, author> lmapauthors;
			for (int i = 100; i < 110; ++i)
			{
				author ltempauthor;
				ltempauthor.name = std::format("caozhen_{}", i);
				ltempauthor.age = i;
				ltempauthor.isVerified = i % 2 == 1;
				lmapauthors[i] = ltempauthor;
			}

			pbdb::db_account ldbtemp;
			ldbtemp.set_mid(1989);
			ldbtemp.set_marea(4);
			ldbtemp.set_mid(3333);
			ldbtemp.set_mpassworld("123456");
			ldbtemp.set_maccount("kangzhilong");


			std::vector<pbdb::db_account> ldbs;
			for (int i = 0; i < 10; ++i)
			{
				pbdb::db_account ldbddtemp;
				ldbddtemp.set_mid(i);
				ldbddtemp.set_marea(100+i);
				ldbddtemp.set_mid(3333);
				ldbddtemp.set_mpassworld("123456");
				ldbddtemp.set_maccount("kangzhilong");
				ldbs.push_back(ldbddtemp);
			}

			njson::push(lnsjon21, { "title", "author", "tags", "rating", "isPublished", "vecauthors", "numbers", "mapauthors", "account", "dbs"}
				, title
				, lauthor
				, tags
				, rating
				, isPublished
				, lvecauthors
				, lnumbers
				, lmapauthors
				, ldbtemp
				, ldbs
			);
			ljsonstrrr = lnsjon21.str();


		}



		{
			ncjson lnsjon21(ljsonstrrr.c_str());
			std::string title;
			std::vector<std::string> tags;
			float rating = 0.f;
			bool isPublished = false;
			author lauthor;
			std::vector<author> lvecauthors;
			std::vector<int> lnumbers;

			std::map<int32_t, author> lmapauthors;

			pbdb::db_account ldbtemp;

			std::vector<pbdb::db_account> ldbs;

			njson::pop(lnsjon21, { "title", "author", "tags", "rating", "isPublished", "vecauthors", "numbers", "mapauthors", "account", "dbs" }
				, title
				, lauthor
				, tags
				, rating
				, isPublished
				, lvecauthors
				, lnumbers
				, lmapauthors
				, ldbtemp
				, ldbs
			);
		}




		
	}
}