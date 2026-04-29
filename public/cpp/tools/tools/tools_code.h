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
// File overview: Declares binary codec helpers for tools.

#pragma once

#include "tools/tools/tools_core.h"

namespace ngl::tools
{
	int varint_length(parm<int32_t>& avalues);
	int varint_length(parm<int64_t>& avalues);

	template <typename T>
	struct varint_parm
	{
		T m_value;
		char* m_buf = nullptr;
		int m_len = 0;
		int32_t* m_bytes = nullptr;
	};

	bool varint_encode(varint_parm<int64_t>& aparm);
	bool varint_decode(varint_parm<int64_t>& aparm);
	bool varint_encode(varint_parm<int32_t>& aparm);
	bool varint_decode(varint_parm<int32_t>& aparm);

	int to_hex(void* apso, int alen, void* apto);
	bool to_bytes(void* apso, int alen, void* apto, int& aout_len);

	void bytexor(char* ap, int32_t aplen, int apos);
}//namespace ngl::tools
