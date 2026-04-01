/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for net.

#pragma once

#include "tools/serialize/structbytes.h"

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
		static std::shared_ptr<pack> npack(bpool* apool, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid, bool ahead)
		{
			// Compute the serialized payload size first so pack::make_pack() can allocate once.
			ngl::ser::serialize_byte lserialize;
			ngl::ser::nserialize::bytes(&lserialize, adata);
			int lbuffbyte = 0;
			if (ahead)
			{
				lbuffbyte = lserialize.pos() + pack_head::size();
				if (lbuffbyte <= 0)
				{
					return nullptr;
				}
			}
			else
			{
				lbuffbyte = lserialize.pos();
				if (lbuffbyte < 0)
				{
					return nullptr;
				}
			}
			std::shared_ptr<pack> lpack = pack::make_pack(apool, lbuffbyte);
			if (lpack == nullptr || lpack->m_buff == nullptr)
			{
				return nullptr;
			}
			// structbytes writes both the pack header and the serialized payload into the buffer.
			if (!structbytes<T>::tobytes(lpack, adata, aactorid, arequestactorid, ahead))
			{
				return nullptr;
			}
			return lpack;
		}
	};
}//namespace ngl
