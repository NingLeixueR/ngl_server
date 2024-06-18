#pragma once

#include <functional>
#include <list>
#include <map>

namespace ngl
{
	///////////////////////////////////////////////////////////
	// 用于提供key/std::function<void(ARG...)>结构
	// 通过成员函数push提供key与std::function<void(ARG...)>
	// 通过成员函数function执行对应的方法
	// TAG标记,无意义,主要用于产生不同类型
	// TKEY用于查找的key类型
	// ...ARG 用于指定std::function<void(ARG...)>的参数
	template <typename TAG, typename TKEY, typename ...ARG>
	class cmd
	{
		cmd() = delete;
		cmd(const cmd&) = delete;
		cmd& operator=(const cmd&) = delete;

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