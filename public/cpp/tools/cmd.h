/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		static std::function<void(ARG...)>& add(const TKEY& akey)
		{
			return m_fun[akey];
		}
	};

	template <typename TAG, typename TKEY, typename ...ARG>
	std::map<TKEY, std::function<void(ARG...)>> cmd<TAG, TKEY, ARG...>::m_fun;
}//namespace ngl