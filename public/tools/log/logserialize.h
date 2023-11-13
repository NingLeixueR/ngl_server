#pragma once

#include <string>
#include <list>
#include <map>
#include <fstream>
#include <inttypes.h>
#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace ngl
{
	class logserialize
	{
		char* m_buff;
		int m_bufflen;
	public:
		logserialize(char* abuff, int abufflen);

		char*	buff();
		int		len();

		bool operator()(int64_t avalue);
		bool operator()(int32_t avalue);
		bool operator()(int16_t avalue);
		bool operator()(int8_t avalue);
		bool operator()(uint64_t avalue);		
		bool operator()(uint32_t avalue);		
		bool operator()(uint16_t avalue);		
		bool operator()(uint8_t avalue);		
		bool operator()(char avalue);		
		bool operator()(float avalue);		
		bool operator()(const double avalue);
		bool operator()(char* avalue);		
		bool operator()(const std::string& avalue);		
		bool operator()(bool avalue);
		bool operator()(const char* avalue);

	private:
		template <typename T>
		bool number(const char* aformat, T adata)
		{
			int lval = snprintf(m_buff, m_bufflen, aformat, adata);
			if (lval <= 0) return false;
			m_buff += lval;
			m_bufflen -= lval;
			return true;
		}
	public:
		template <typename TKEY, typename TVALUE>
		bool operator()(const std::pair<TKEY, TVALUE>& adata)
		{
			if (!(*this)("pair<"))			
				return false;
			if (!(*this)(adata.first))	
				return false;
			if (!(*this)(","))			
				return false;
			if (!(*this)(adata.second))	
				return false;
			if (!(*this)(">"))			
				return false;
			return true;
		}

		template <typename TKEY, typename TVALUE>
		bool operator()(const std::map<TKEY, TVALUE>& avalue)
		{
			if (!(*this)("map{"))	
				return false;
			for (const std::pair<const TKEY, TVALUE>& item : avalue)
			{
				if (!(*this)("["))			
					return false;
				if (!(*this)(item.first))	
					return false;
				if (!(*this)(","))			
					return false;
				if (!(*this)(item.second))	
					return false;
				if (!(*this)("]"))			
					return false;
			}
			if (!(*this)("}"))	
				return false;
			return true;
		}

		template <typename T>
		bool operator()(const std::vector<T>& avalue)
		{
			if (!(*this)("vector["))	
				return false;
			for (int i = 0; i < avalue.size(); ++i)
			{
				if (!(*this)(avalue[i]))	
					return false;
				if (i + 1 < avalue.size())
					if (!(*this)(","))	
						return false;
			}
			if (!(*this)("]"))	
				return false;
			return true;
		}

		template <typename T>
		bool operator()(const std::list<T>& avalue)
		{
			int lcount = 0;
			if (!(*this)("list["))	
				return false;
			for (const T& item : avalue)
			{
				if (!(*this)(item))	
					return false;
				if (++lcount < avalue.size())
					if (!(*this)(","))	
						return false;
			}
			if (!(*this)("]"))	
				return false;
			return true;
		}

		template <typename T>
		bool operator()(const std::set<T>& avalue)
		{
			int lcount = 0;
			if (!(*this)("set["))	
				return false;
			for (const T& item : avalue)
			{
				if (!(*this)(item))		
					return false;
				if (++lcount < avalue.size())
					if (!(*this)(","))	
						return false;
			}
			if (!(*this)("]"))	
				return false;
			return true;
		}

		template <typename T>
		bool operator()(const T& avalue);

		template <typename T>
		bool operator()(const boost::shared_ptr<T>& adata)
		{
			return adata->log(*this);
		}

		template <typename T>
		bool operator()(const T* avalue)
		{
			return avalue->log(*this);
		}

		template <typename T>
		bool operator()(T* avalue)
		{
			return avalue->log(*this);
		}

		template <typename T>
		bool operator()()
		{
			return true;
		}

		template <typename T, typename ...ARG>
		bool operator()(const T& avalue, const ARG& ... arg)
		{
			return (*this)(avalue) && (*this)("*") && (*this)(arg...);
		}
	};

}