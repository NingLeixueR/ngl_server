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


#include "tools/serialize/netbuff_pool.h"
#include "actor/protocol/nprotocol.h"
#include "tools/serialize/pack.h"

#include <new>

namespace ngl
{
	char* bpool::malloc(int32_t alen)
	{
		return netbuff_pool::malloc(alen);
	}

	void bpool::free(char* ap, int32_t alen)
	{
		netbuff_pool::free(ap);
		return;
	}

	void pack::set(bpool& apool) 
	{ 
		m_bpool = &apool; 
	}

	void pack::reset()
	{
		free();
		m_protocol = ENET_TCP;
		m_id = 0;
		m_len = 0;
		m_pos = 0;
		m_rate_accounted = false;
		m_head.reset();
	}

	bool pack::isready()
	{
		if (m_head.isready() == EPH_HEAD_SUCCESS)
		{
			return m_pos >= m_head.get_bytes();
		}
		return false;
	}

	bool pack::malloc(int32_t alen)
	{
		if (m_buff != nullptr)
		{
			free();
		}
		if (alen <= 0)
		{
			m_len = 0;
			m_pos = 0;
			m_rate_accounted = false;
			return false;
		}
		if (m_bpool == nullptr)
		{
			// Fallback allocation is only used outside the shared network pool,
			// mainly in tests or standalone utility code.
			m_buff = new(std::nothrow) char[alen];
		}
		else
		{
			m_buff = m_bpool->malloc(alen);
		}
		if (m_buff == nullptr)
		{
			m_len = 0;
			m_pos = 0;
			m_rate_accounted = false;
			return false;
		}
		m_len = alen;
		m_pos = 0;
		m_rate_accounted = false;
		return true;
	}

	void pack::free()
	{
		if (m_buff == nullptr)
		{
			return;
		}
		if (m_bpool != nullptr)
		{
			m_bpool->free(m_buff, m_len);
		}
		else
		{
			delete[] m_buff;
		}
		m_buff = nullptr;
		m_len = 0;
		m_pos = 0;
		m_rate_accounted = false;
	}

	void pack::set_actor(i64_actorid aactor, i64_actorid arequestactorid)
	{
		if (m_buff == nullptr || m_len < pack_head::size())
		{
			return;
		}
		// Rewrite the actor fields in-place after the packet body has already
		// been serialized.
		pack_head::head_set_actor((int32_t*)m_buff, aactor, arequestactorid);
	}

	pack::~pack()
	{
		Try
		{
			free();
		}Catch
	}

	std::shared_ptr<pack> pack::make_pack(bpool* apool, int32_t alen)
	{
		if (alen < 0)
		{
			return nullptr;
		}
		std::shared_ptr<pack> lpack = std::make_shared<pack>();
		if (apool != nullptr)
		{
			lpack->set(*apool);
		}
		if (alen > 0)
		{
			if (!lpack->malloc(alen))
			{
				return nullptr;
			}
		}
		return lpack;
	}
}// namespace ngl
