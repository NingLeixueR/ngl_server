#pragma once

#include <functional>
#include <map>

namespace ngl
{
	template <typename TAG, typename ...ARG>
	class cmd
	{
		static std::map<std::string, std::function<void(ARG...)>> m_fun;
	public:
		static bool empty()
		{
			return m_fun.empty();
		}

		static void push(const std::string& akey, const std::function<void(ARG...)>& afun)
		{
			m_fun[akey] = afun;
		}

		static bool function(const std::string& akey, ARG... arg)
		{
			auto itor = m_fun.find(akey);
			if (itor == m_fun.end())
			{
				return false;
			}
			itor->second(arg...);
		}
	};

	template <typename TAG, typename ...ARG>
	std::map<std::string, std::function<void(ARG...)>> cmd<TAG, ARG...>::m_fun;
}