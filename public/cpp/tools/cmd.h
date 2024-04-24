#pragma once

#include <functional>
#include <map>

namespace ngl
{
	template <typename TAG, typename TKEY, typename ...ARG>
	class cmd
	{
		static std::map<TKEY, std::function<void(ARG...)>> m_fun;
	public:
		static bool empty()
		{
			return m_fun.empty();
		}

		static void push(const TKEY& akey, const std::function<void(ARG...)>& afun)
		{
			m_fun[akey] = afun;
		}

		static bool function(const TKEY& akey, ARG... arg)
		{
			auto itor = m_fun.find(akey);
			if (itor == m_fun.end())
			{
				return false;
			}
			itor->second(arg...);
			return true;
		}
	};

	template <typename TAG, typename TKEY, typename ...ARG>
	std::map<TKEY, std::function<void(ARG...)>> cmd<TAG, TKEY, ARG...>::m_fun;
}