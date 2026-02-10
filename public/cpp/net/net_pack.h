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