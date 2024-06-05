#pragma once

#include "serialize.h"
#include "type.h"
#include "db.h"

#ifdef USE_BOOST_INTERPROCESS
#include "shared_memory.h"
#endif//USE_BOOST_INTERPROCESS

#include <set>

namespace ngl
{
	template <typename T>
	struct db_data
	{
	private:

#ifdef USE_BOOST_INTERPROCESS
		static shared_memory<T>				m_data;
#else
		static std::map<i64_actorid, T>		m_data;
#endif//USE_BOOST_INTERPROCESS
	public:
		static void init()
		{
#ifdef USE_BOOST_INTERPROCESS
			m_data.init();
#endif//USE_BOOST_INTERPROCESS	
		}

		static bool set(i64_actorid aid, const T& adata)
		{
#ifdef USE_BOOST_INTERPROCESS
			return m_data.set(aid, adata);
#else
			m_data[aid] = adata;
			return true;
#endif//USE_BOOST_INTERPROCESS			
		}

		static void remove(i64_actorid aid)
		{
#ifdef USE_BOOST_INTERPROCESS
			return m_data.remove(aid);
#else
			m_data.erase(aid);
#endif//USE_BOOST_INTERPROCESS
		}

		static void remove(std::set<i64_actorid>& aid)
		{
#ifdef USE_BOOST_INTERPROCESS
			return m_data.remove(aid);
#else
			for (i64_actorid id : aid)
				m_data.erase(id);
#endif//USE_BOOST_INTERPROCESS
		}

		static void remove(std::vector<i64_actorid>& aid)
		{
#ifdef USE_BOOST_INTERPROCESS
			return m_data.remove(aid);
#else
			for (i64_actorid id : aid)
				m_data.erase(id);
#endif//USE_BOOST_INTERPROCESS	
		}

		static bool isload(i64_actorid aid)
		{
			return false;
#ifdef USE_BOOST_INTERPROCESS
			return m_data.get(aid) != nullptr;
#else
			auto itor = m_data.find(aid);
			if (itor == m_data.end())
				return false;
			return true;
#endif//USE_BOOST_INTERPROCESS	
		}

		static T* find(i64_actorid aid)
		{
			return nullptr;
#ifdef USE_BOOST_INTERPROCESS
			return m_data.get(aid);
#else
			auto itor = m_data.find(aid);
			if (itor == m_data.end())
				return nullptr;
			return &itor->second;
#endif//USE_BOOST_INTERPROCESS	
		}

		static bool get(i64_actorid aid, T& adata)
		{
			return false;
#ifdef USE_BOOST_INTERPROCESS
			T* lp = m_data.get(aid);
			if (lp == nullptr)
				return false;
			*lp = adata;
			return true;
#else
			auto itor = m_data.find(aid);
			if (itor == m_data.end())
				return false;
			adata = itor->second;
			return true;
#endif//USE_BOOST_INTERPROCESS
		}

		static void foreach(const std::function<void(T&)>& afun)
		{
#ifdef USE_BOOST_INTERPROCESS
			m_data.foreach(afun);
#else
			for (std::pair<const i64_actorid, T>& item : m_data)
				afun(item.second);
#endif//USE_BOOST_INTERPROCESS
		}

		static void foreach_index(const std::function<void(int, T&)>& afun)
		{
#ifdef USE_BOOST_INTERPROCESS
			m_data.foreach_index(afun);
#else
			int lindex = 0;
			for (std::pair<const i64_actorid, T>& lpair : m_data)
				afun(++lindex, lpair.second);
#endif//USE_BOOST_INTERPROCESS
		}
	};

#ifdef USE_BOOST_INTERPROCESS
	template <typename T>
	shared_memory<T> db_data<T>::m_data;
#else
	template <typename T>
	std::map<i64_actorid, T> db_data<T>::m_data;
#endif//USE_BOOST_INTERPROCESS

}// namespace ngl


