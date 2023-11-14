#include "autoitem.h"
#include "actor_role.h"

namespace ngl
{
	void aitem::add(int32_t atid, int32_t acount)
	{
		m_addstackitems[atid] += acount;
	}

	void aitem::add(int32_t aid)
	{
		m_addnostackitems.push_back(aid);
	}

	void aitem::del(int32_t aid, int32_t acount)
	{
		m_delstackitems[aid] += acount;
	}

	void aitem::del(int32_t aid)
	{
		m_addnostackitems.push_back(aid);
	}

	autoitem::~autoitem()
	{
		// #### 自动同步背包状态给客户端
		auto pro = std::make_shared<pbnet::PROBUFF_NET_BAG_UPDATE_RESPONSE>();
		for (const auto& [_id, _count] : m_role->m_aitem.m_delstackitems)
		{

		}
		auto delitem = pro->add_m_delitems();
		//delitem->
	}

	bool autoitem::add_item(int32_t atid, int32_t acount)
	{
		return m_role->m_bag.add_item(&m_data, atid, acount);
	}

	bool autoitem::add_item(const std::map<int32_t, int32_t>& amap)
	{
		return m_role->m_bag.add_item(&m_data, amap);
	}

	bool autoitem::add_item(std::vector<pbdb::item>& avec)
	{
		return m_role->m_bag.add_item(&m_data, avec);
	}

	bool autoitem::dec_item(int32_t atid, int32_t acount)
	{
		return m_role->m_bag.dec_item(&m_data, atid, acount);
	}

	bool autoitem::dec_item(int32_t aid)
	{
		return m_role->m_bag.dec_item(&m_data, aid);
	}
}