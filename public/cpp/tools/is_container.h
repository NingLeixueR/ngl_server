#pragma once

#include <string>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>

namespace ngl
{
	// 默认类型为false
	template<typename T, typename... Types>
	struct is_containery_type
	{
		static const bool value = false;
	};

	template<>
	struct is_containery_type<std::string>
	{
		static const bool value = true;
	};

	// Vector类型为true
	template<typename T, typename... Types>
	struct is_containery_type<std::vector<T, Types...>>
	{
		static const bool value = true;
	};
	// Vector类型为true
	template<typename T, typename... Types>
	struct is_containery_type<std::list<T, Types...>>
	{
		static const bool value = true;
	};

	// deque类型
	template<typename T, typename... Types>
	struct is_containery_type<std::deque<T, Types...>>
	{
		static const bool value = true;
	};

	// set类型
	template<typename T, typename... Types>
	struct is_containery_type<std::set<T, Types...>>
	{
		static const bool value = true;
	};

	// multiset类型
	template<typename T, typename... Types>
	struct is_containery_type<std::multiset<T, Types...>>
	{
		static const bool value = true;
	};

	// map类型
	template<typename K, typename V, typename... Types>
	struct is_containery_type<std::map<K, V, Types...>>
	{
		static const bool value = true;
	};

	// multimap类型
	template<typename K, typename V, typename... Types>
	struct is_containery_type<std::multimap<K, V, Types...>>
	{
		static const bool value = true;
	};

	// 定义获取容器类型的模板
	template<typename T, typename... Types>
	constexpr bool is_container = is_containery_type<T, Types...>::value;
}// namespace ngl