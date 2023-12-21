#pragma once 

#include "actor_enum.h"

namespace ngl
{
	template <typename T>
	class actor_type
	{
		static ENUM_ACTOR m_type;
	public:
		static void inits(ENUM_ACTOR atype)
		{
			m_type = atype;
		}

		static ENUM_ACTOR type()
		{
			assert(m_type == ACTOR_NONE);
			return m_type;
		}
	};

	template <typename T>
	ENUM_ACTOR actor_type<T>::m_type = ACTOR_NONE;

	void init_actor_type();
}