/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/

#include "ndefine.h"
#include "db.pb.h"

namespace ngl
{
	struct bbbbb
	{
		int32_t m_1;
		bool m_2;
		std::vector<int32_t> m_3;
		pbdb::db_family m_4;
		std::list<pbdb::db_family> m_5;

		//def_jsonfunction_special_parm("m_1", m_1, "m_2", m_2, "m_3", m_3)
		def_parmname
		def_jsonfunction(m_1, m_2, m_3, m_4, m_5)
	};

	struct abcccc
	{
		int32_t m_1;
		bool m_2;
		std::vector<int32_t> m_3;
		std::vector<bbbbb> m_4;

		def_jsonfunction_special_parm("m_1", m_1, "m_2", m_2, "m_3", m_3, "m_4", m_4)

	};


	void test_json()
	{
		abcccc ltemp1;
		ltemp1.m_1 = 1989;
		ltemp1.m_2 = false;
		ltemp1.m_3 = {1,9,8,9,0,5,1,9};

		int lv = 1;
		for(int i = 0;i<5;++i, lv*=10)
		{
			bbbbb ltemp;
			ltemp.m_1 = 1* lv;
			ltemp.m_2 = true;
			ltemp.m_3 = { 2 * lv,0 * lv,2 * lv,6 * lv,0,1,0,5 };
			ltemp.m_4.set_mid(111 * lv);
			ltemp.m_4.set_mcreateutc(222 * lv);
			ltemp.m_4.set_mname("libo");
			ltemp.m_4.set_mleader(333 * lv);
			ltemp.m_4.set_mlv(444 * lv);
			ltemp.m_4.set_mexp(555 * lv);
			ltemp.m_4.add_mmember(666 * lv);
			ltemp.m_4.add_mmember(777 * lv);
			ltemp.m_4.add_mapplylist(888 * lv);
			ltemp.m_4.add_mapplylist(999 * lv);
			ltemp1.m_4.push_back(ltemp);
		}

		ncjson lncjson;
		njson::push(lncjson.json(), "abcccc", ltemp1);
		std::string ltempstr = lncjson.str();
		std::cout << ltempstr << std::endl;

		abcccc ltemp2;
		njson::pop(lncjson.json(), "abcccc", ltemp2);
	}

}