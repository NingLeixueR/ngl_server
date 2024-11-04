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
		static std::shared_ptr<pack> npack(bpool* apool, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			ngl::serialize_bytes lbytes;
			int lbuffbyte = adata.ByteSize() + pack_head::size();
			std::shared_ptr<pack> lpack = pack::make_pack(apool, lbuffbyte);
			if (structbytes<T>::tobytes(lpack, adata, aactorid, arequestactorid) == false)
				return nullptr;
			return lpack;
		}
	};
}//namespace ngl