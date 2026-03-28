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

#include <cstring>
#include <new>
#include <utility>

namespace ngl
{
	namespace pack_detail
	{
		static std::shared_ptr<pack_head> make_head()
		{
			return std::make_shared<pack_head>();
		}
	}

	char* bpool::malloc(int32_t alen)
	{
		return netbuff_pool::malloc(alen);
	}

	void bpool::free(char* ap, [[maybe_unused]] int32_t alen)
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
		// clone() shares the header pointer, so reset() must replace it instead
		// of mutating a header that may still be owned by another pack.
		m_head = pack_detail::make_head();
	}

	bool pack::isready()
	{
		if (m_head != nullptr && m_head->isready() == EPH_HEAD_SUCCESS)
		{
			return m_pos >= m_head->get_bytes();
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
		std::shared_ptr<char[]> lhold = nullptr;
		if (m_bpool == nullptr)
		{
			char* lbuf = new (std::nothrow) char[alen];
			if (lbuf != nullptr)
			{
				lhold = std::shared_ptr<char[]>(lbuf, std::default_delete<char[]>());
			}
		}
		else
		{
			char* lbuf = m_bpool->malloc(alen);
			if (lbuf != nullptr)
			{
				bpool* lpool = m_bpool;
				lhold = std::shared_ptr<char[]>(lbuf, [lpool, alen](char* ap)
					{
						if (ap != nullptr && lpool != nullptr)
						{
							lpool->free(ap, alen);
						}
					}
				);
			}
		}
		m_hold = std::move(lhold);
		m_buff = m_hold.get();
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
		m_hold.reset();
		m_buff = nullptr;
		m_len = 0;
		m_pos = 0;
		m_rate_accounted = false;
	}

	void pack::set_actor(i64_actorid aactor, i64_actorid arequestactorid)
	{
		copy_head(aactor, arequestactorid);
	}

	void pack::copy_head(i64_actorid aactor, i64_actorid arequestactorid)
	{
		std::shared_ptr<pack_head> lhead = m_head != nullptr ? std::make_shared<pack_head>(*m_head) : pack_detail::make_head();
		lhead->set_actor(aactor, arequestactorid);
		m_head = lhead;
	}

	bool pack::has_head() const
	{
		return m_head != nullptr && pack_head::head_check_mask(m_head->m_data, pack_head::size()) == EPH_HEAD_MASK_SUCCESS;
	}

	const char* pack::body_ptr() const
	{
		if (m_buff == nullptr)
		{
			return nullptr;
		}
		return m_buff;
	}

	int32_t pack::body_len() const
	{
		return m_len;
	}

	void pack::head_copy(char* abuff) const
	{
		if (abuff == nullptr)
		{
			return;
		}
		if (m_head == nullptr)
		{
			memset(abuff, 0, static_cast<std::size_t>(pack_head::size()));
			return;
		}
		memcpy(abuff, m_head->m_data, static_cast<std::size_t>(pack_head::size()));
	}

	std::shared_ptr<pack> pack::clone() const
	{
		std::shared_ptr<pack> lpack = make_pack(m_bpool, 0);
		if (lpack == nullptr)
		{
			return nullptr;
		}

		lpack->m_protocol = m_protocol;
		lpack->m_id = m_id;
		lpack->m_head = m_head;
		lpack->m_rate_accounted = m_rate_accounted;
		lpack->m_hold = m_hold;
		lpack->m_buff = lpack->m_hold.get();
		lpack->m_len = m_len;
		lpack->m_pos = m_pos;
		return lpack;
	}

	std::shared_ptr<pack> pack::clone_actor(i64_actorid aactor, i64_actorid arequestactorid) const
	{
		std::shared_ptr<pack> lpack = clone();
		if (lpack == nullptr)
		{
			return nullptr;
		}
		lpack->set_actor(aactor, arequestactorid);
		return lpack;
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
