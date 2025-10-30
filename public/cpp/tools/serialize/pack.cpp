#include "netbuff_pool.h"
#include "segpack.h"
#include "pack.h"

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
		m_id = 0;
		m_buff = nullptr;
		m_len = 0;
		m_pos = 0;
		m_segpack = nullptr;
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
		if (m_bpool == nullptr)
		{
			m_buff = new char[alen];
		}
		else
		{
			m_buff = m_bpool->malloc(alen);
		}
		m_len = alen;
		m_pos = 0;
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
	}

	void pack::set_actor(i64_actorid aactor, i64_actorid arequestactorid)
	{
		if (m_buff == nullptr)
		{
			return;
		}
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
		std::shared_ptr<pack> lpack = std::make_shared<pack>();
		lpack->set(*apool);
		if (alen > 0)
		{
			lpack->malloc(alen);
		}
		return lpack;
	}
}// namespace ngl