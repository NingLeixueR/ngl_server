/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "structbytes.h"

namespace ngl
{
	template <typename T>
	class net_pack
	{
		net_pack() = delete;
		net_pack(const net_pack&) = delete;
		net_pack& operator=(const net_pack&) = delete;
	public:
		template <typename Y>
		static std::shared_ptr<pack> npack(bpool* apool, Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			ngl::ser::serialize_byte lserialize;
			ngl::ser::nserialize::bytes(&lserialize, adata);
			int lbuffbyte = lserialize.pos() + pack_head::size();
			std::shared_ptr<pack> lpack = pack::make_pack(apool, lbuffbyte);
			if (structbytes<T>::tobytes(lpack, adata, aactorid, arequestactorid) == false)
			{
				return nullptr;
			}
			return lpack;
		}
	};
}//namespace ngl