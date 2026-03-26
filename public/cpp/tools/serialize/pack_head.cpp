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
// File overview: Implements logic for serialize.


#include "tools/serialize/pack_head.h"
#include "actor/actor_base/core/nguid.h"
#include "tools/tab/xml/xml.h"
#include "tools/log/nlog.h"

#include <algorithm>
#include <cstring>

namespace ngl
{
	namespace
	{
		template <typename T>
		void copy_to_head(int32_t* abuff, EPH aoffset, const T& avalue)
		{
			// The header layout is defined in 32-bit slots, but some logical
			// fields span multiple slots and are copied as raw bytes.
			memcpy(&abuff[aoffset], &avalue, sizeof(avalue));
		}

		template <typename T>
		T copy_from_head(const int32_t* abuff, EPH aoffset)
		{
			T lvalue{};
			memcpy(&lvalue, &abuff[aoffset], sizeof(lvalue));
			return lvalue;
		}
	}

	pack_head::pack_head() :
		m_wpos(0)
	{
	}

	void pack_head::head_set_actor(
		int32_t* abuff,
		i64_actorid aactor,
		i64_actorid arequestactorid
	)
	{
		copy_to_head(abuff, EPH::EPH_ACTOR_TYPEAREA, aactor);
		copy_to_head(abuff, EPH::EPH_REQUEST_ACTOR_TYPEAREA, arequestactorid);
	}

	void pack_head::head_set_actor(int32_t* abuff, i64_actorid aactor)
	{
		copy_to_head(abuff, EPH::EPH_ACTOR_TYPEAREA, aactor);
	}

	void pack_head::set_actor(i64_actorid aactor, i64_actorid arequestactorid)
	{
		head_set_actor(m_data, aactor, arequestactorid);
	}

	void pack_head::set_requestactor(i64_actorid arequestactor)
	{
		copy_to_head(m_data, EPH::EPH_REQUEST_ACTOR_TYPEAREA, arequestactor);
	}

	void pack_head::set_time()
	{
		head_set_time(m_data);
	}

	void pack_head::head_set_time(int32_t* abuff)
	{
		abuff[EPH::EPH_TIME] = (int32_t)localtime::gettime();
	}

	i32_time pack_head::head_get_time(const int32_t* abuff)
	{
		return abuff[EPH::EPH_TIME];
	}

	enum
	{
		EPH_MASK_COUNT_BYTES = EPH_MASK_COUNT * sizeof(int32_t)
	};

	void pack_head::head_set_mask(int32_t* abuff)
	{
		auto* lpbuff = reinterpret_cast<uint8_t*>(abuff);
		for (int i = 0; i < EPH_MASK_COUNT_BYTES; ++i)
		{
			lpbuff[i] = static_cast<uint8_t>(EPH::EPH_MASK_VALUE);
		}
	}

	EPH_HEAD_VAL pack_head::head_check_mask(const int32_t* abuff, int awpos)
	{
		int lnum = awpos;
		if (lnum > EPH_MASK_COUNT_BYTES)
		{
			lnum = EPH_MASK_COUNT_BYTES;
		}
		auto* lpbuff = (const uint8_t*)abuff;
		for (int i = 0; i < lnum; ++i)
		{
			if (lpbuff[i] != EPH::EPH_MASK_VALUE)
			{
				return EPH_HEAD_MASK_FAIL;
			}
		}
		return EPH_HEAD_MASK_SUCCESS;
	}

	void pack_head::set_mask()
	{
		head_set_mask(m_data);
	}

	EPH_HEAD_VAL pack_head::check_mask()const
	{
		return head_check_mask(m_data, m_wpos);
	}

	i32_time pack_head::get_time()const
	{
		return head_get_time(m_data);
	}

	i64_actorid pack_head::get_actor()const
	{
		return copy_from_head<i64_actorid>(m_data, EPH::EPH_ACTOR_TYPEAREA);
	}

	i64_actorid pack_head::get_request_actor()const
	{
		return copy_from_head<i64_actorid>(m_data, EPH::EPH_REQUEST_ACTOR_TYPEAREA);
	}

	i16_actortype pack_head::get_actortype()const
	{
		return nguid::type(get_actor());
	}

	i16_actortype pack_head::get_request_actortype()const
	{
		return nguid::type(get_request_actor());
	}

	i16_area pack_head::get_actorarea()const
	{
		return nguid::area(get_actor());
	}

	i32_actordataid pack_head::get_actordataid()const
	{
		return nguid::actordataid(get_actor());
	}

	void pack_head::reset()
	{
		memset(&m_data, 0x0, sizeof(m_data));
		m_wpos = 0;
	}

	int32_t pack_head::getvalue(EPH aeph)const
	{
		return m_data[aeph];
	}

	int32_t pack_head::get_bytes()const
	{
		return m_data[EPH::EPH_BYTES];
	}

	int32_t pack_head::size()
	{
		return (int32_t)sizeof(m_data);
	}

	EPH_HEAD_VAL pack_head::isready()const
	{
		if (check_mask() == EPH_HEAD_MASK_FAIL)
		{
			return EPH_HEAD_MASK_FAIL;
		}
		if (m_wpos >= size())
		{
			return EPH_HEAD_SUCCESS;
		}
		return EPH_HEAD_FOLLOW;
	}

	i32_protocolnum pack_head::protocolnum()const
	{
		return m_data[EPH::EPH_PROTOCOLNUM];
	}

	i32_protocolnum pack_head::get_protocolnumber()const
	{
		return protocolnum();
	}

	void pack_head::set_protocol(i32_protocolnum aprotocolnum)
	{
		m_data[EPH::EPH_PROTOCOLNUM] = aprotocolnum;
	}

	EPH_HEAD_VAL pack_head::push(const char*& abuff, int32_t& alen)
	{
		if (check_mask() == EPH_HEAD_MASK_FAIL)
		{
			return EPH_HEAD_MASK_FAIL;
		}
		if (alen <= 0)
		{
			return isready();
		}
		if (abuff == nullptr)
		{
			return EPH_HEAD_MASK_FAIL;
		}

		const int32_t ltemp = std::min(alen, size() - m_wpos);
		if (ltemp <= 0)
		{
			return isready();
		}

		// Headers can arrive fragmented across multiple socket reads.
		char* lp = (char*)m_data;
		memcpy(&lp[m_wpos], abuff, ltemp);
		alen -= ltemp;
		abuff += ltemp;
		m_wpos += ltemp;
		return isready();
	}

	bool pack_head::push_format(ngl::ser::serialize_push* aserialize)const
	{
		return aserialize->basetype((void*)m_data, sizeof(m_data));
	}

	void pack_head::reservebuff(char* abuff, int abufflen, std::pair<char*, int32_t>& apair)
	{
		if (abuff == nullptr || abufflen < size())
		{
			apair.first = nullptr;
			apair.second = 0;
			return;
		}
		// Reserve the body span behind the fixed-size packet header.
		apair.first = &(abuff[size()]);
		apair.second = abufflen - size();
	}
}// namespace ngl
