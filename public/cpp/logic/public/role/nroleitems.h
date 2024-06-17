#pragma once

#include "ndb_modular.h"

namespace ngl
{
	//# nroleitems<TDerived,T>
	//# 为actor对象TDerived提供对
	//# std::vector<pbdb::db_brief>的离线数据同步
	template <typename TDerived, typename T>
	struct nroleitems
	{
		nroleitems() = delete;
		nroleitems(const nroleitems&) = delete;
		nroleitems& operator=(const nroleitems&) = delete;

		static std::map<i64_actorid, T> m_roleitem;
	public:
		static void init(
			const std::function<void(std::map<i64_actorid, T>&, pbdb::db_brief&)>& afun
		)
		{
			// 注册协议
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, 
				TDerived, 
				np_actor_roleinfo
			>([afun](TDerived* apTDerived, np_actor_roleinfo& ainfo)
				{
					for (pbdb::db_brief& item : *ainfo.m_vecinfo.m_data)
					{
						afun(m_roleitem, item);
					}
				});
		}

		static T* get_roleinfo(i64_actorid aid)
		{
			auto itor = m_roleitem.find(aid);
			if (itor == m_roleitem.end())
				return nullptr;
			return &itor->second;
		}
	};

	template <typename TDerived, typename T>
	std::map<i64_actorid, T> nroleitems<TDerived, T>::m_roleitem;
}//namespace ngl