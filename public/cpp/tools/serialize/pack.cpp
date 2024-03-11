#include "netbuff_pool.h"
#include "segpack.h"
#include "pack.h"

namespace ngl
{
	bpool::bpool()
	{}

	char* bpool::malloc(int alen)
	{
		return netbuff_pool::malloc(alen);
	}

	void bpool::free(char* ap, int alen)
	{
		netbuff_pool::free(ap);
		return;
	}

	pack::~pack() {}

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
		return m_head.isready() == EPH_HEAD_SUCCESS ? m_pos >= m_head.get_bytes() /*+ EPH_SUM * sizeof(uint32_t)*/ : false;
	}

	bool pack::malloc(int alen)
	{
		if (m_buff != nullptr)
			free();
		m_buff = (m_bpool == nullptr)? new char[alen]: m_bpool->malloc(alen);
		m_len = alen;
		m_pos = 0;
		return true;
	}

	void pack::free()
	{
		if (m_buff != nullptr)
		{
			if (m_bpool != nullptr)
				m_bpool->free(m_buff, m_len);
			else
				delete[] m_buff;
			m_buff = nullptr;
			m_len = 0;
		}
	}

	void pack::set_actor(i64_actorid aactor, i64_actorid arequestactorid)
	{
		if (m_buff == nullptr)
			return;
		pack_head::head_set_actor((int32_t*)m_buff, aactor, arequestactorid);
	}

	std::shared_ptr<pack> pack::make_pack(bpool* apool, int alen)
	{
		std::shared_ptr<pack> lpack(new pack(), [](pack* apack)
			{
				apack->free();
				delete apack;
			});
		lpack->set(*apool);
		if (alen > 0)
			lpack->malloc(alen);
		return lpack;
	}
}// namespace ngl