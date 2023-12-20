#pragma once 

#include "actor_enum.h"

#ifndef NOTUSING_BOOST_TYPEINFO 
# include <boost/type_index.hpp>
# define HASH_CODE_VALUE(_TYPE)	boost::typeindex::type_id<_TYPE>().hash_code()
#else
# define HASH_CODE_VALUE(_TYPE)	typeid(T).hash_code()
#endif

namespace ngl
{
	class actor_type
	{
		template <typename T>
		static size_t hash_code()
		{
			static size_t lcode = HASH_CODE_VALUE(T);
			return lcode;
		}
		static std::multimap<size_t, ENUM_ACTOR> m_keyval;

		

	public:
		template <typename T>
		static void inits(ENUM_ACTOR atype)
		{
			size_t lcode = hash_code<T>();
			if (m_keyval.find(lcode) != m_keyval.end())
			{
				return;
			}
			m_keyval.insert(std::make_pair(lcode, atype));
		}


		template <typename T>
		static ENUM_ACTOR type()
		{
			using TRC = std::remove_const<T>::type;
			size_t lcode = hash_code<T>();
			auto itor = m_keyval.find(lcode);
			assert(itor != m_keyval.end());
			return itor->second;
		}

		static void init();
	};


}