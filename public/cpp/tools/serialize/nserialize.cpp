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
#include "nserialize.h"
#include "db.pb.h"

namespace ngl
{
	namespace ser
	{
		struct abcccc1
		{
			int m_1 = 2;
			float m_2 = 2.0f;
		};
		struct abcccc2
		{
			int m_1 = 1;
			float m_2 = 1.0f;
			abcccc1 m_3;
		};

		template <>
		struct serialize_format<abcccc1>
		{
			static bool push(serialize_push* aserialize, const abcccc1& adata)
			{
				return nserialize::push(aserialize, adata.m_1, adata.m_2);
			}

			static bool pop(serialize_pop* aserialize, abcccc1& adata)
			{
				return nserialize::pop(aserialize, adata.m_1, adata.m_2);
			}

			static void bytes(serialize_byte* aserialize, const abcccc1& adata)
			{
				nserialize::bytes(aserialize, adata.m_1, adata.m_2);
			}
		};

		template <>
		struct serialize_format<abcccc2>
		{
			static bool push(serialize_push* aserialize, const abcccc2& adata)
			{
				return nserialize::push(aserialize, adata.m_1, adata.m_2, adata.m_3);
			}

			static bool pop(serialize_pop* aserialize, abcccc2& adata)
			{
				return nserialize::pop(aserialize, adata.m_1, adata.m_2, adata.m_3);
			}

			static void bytes(serialize_byte* aserialize, const abcccc2& adata)
			{
				nserialize::bytes(aserialize, adata.m_1, adata.m_2, adata.m_3);
			}
		};

		void test_serialize()
		{
			char lbuff[1024] = { 0 };
			serialize_push lpush(lbuff, 1024);
			int32_t lv1 = 123;
			int32_t lv2 = 456.01f;
			std::map<int, std::string> lmap =
			{
				{1, "s1"},
				{2, "s2"},
				{3, "s3"},
				{4, "s4"},
			};

			abcccc2 ltemp;
			ltemp.m_1 = 777;
			ltemp.m_2 = 888.8f;
			ltemp.m_3.m_1 = 999;
			ltemp.m_3.m_2 = 333.3f;

			serialize_byte lbyte;
			pbdb::db_account laccount;
			laccount.set_mid(999);
			laccount.set_marea(888);
			laccount.set_maccount("libo");
			laccount.set_mpassworld("123456");
			nserialize::bytes(&lbyte, lv1, lv2, lmap, ltemp, laccount);
			std::cout << lbyte.pos() << std::endl;

			nserialize::push(&lpush, lv1, lv2, lmap, ltemp, laccount);


			serialize_pop lpop(lpush.buff(), lpush.pos());
			int32_t lvv1 = 0;
			int32_t lvv2 = 0.0f;
			std::map<int, std::string> lvmap;

			abcccc2 lvtemp;
			pbdb::db_account laccount2;

			nserialize::pop(&lpop, lvv1, lvv2, lvmap, lvtemp, laccount2);
		}
	}
}// namespace ngl