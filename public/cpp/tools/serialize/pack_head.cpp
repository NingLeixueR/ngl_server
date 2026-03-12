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

#include "tools/serialize/pack_head.h"
#include "actor/actor_base/nguid.h"
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
		auto* lpbuff = (char*)abuff;
		for (int i = 0; i < EPH_MASK_COUNT_BYTES; ++i)
		{
			lpbuff[i] = (char)EPH::EPH_MASK_VALUE;
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
		apair.first = &(abuff[size()]);
		apair.second = abufflen - size();
	}
}// namespace ngl
