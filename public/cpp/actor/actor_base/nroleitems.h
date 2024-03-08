#pragma once

#include "ndb_modular.h"

namespace ngl
{
	template <typename TDerived, typename T>
	struct nroleitems
	{
		static std::map<i64_actorid, T> m_roleitem;
	public:
		static void init(const std::function<void(std::map<i64_actorid, T>&, pbdb::db_brief&)>& afun)
		{
			// ×¢²áÐ­Òé
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, actor_roleinfo>([afun](TDerived* apTDerived,actor_roleinfo& ainfo)
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