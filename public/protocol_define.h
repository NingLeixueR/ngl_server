#pragma once

#include "is_container.h"


#define def_protocol_member(NAME, INDEX)			\
			public:									\
				auto& m##NAME()						\
				{									\
					change();						\
					return NAME;					\
				}									\
				const auto& const_m##NAME()const	\
				{									\
					return NAME;					\
				}									\
				const char* name_##NAME()const		\
				{									\
					return #NAME;					\
				}



namespace ngl
{
	struct operator_protocol
	{
	protected:
		bool m_change;

		operator_protocol():
			m_change(false)
		{}

	public:
		void change()
		{
			m_change = true;
		}

		bool is_change()const
		{
			return m_change;
		}

		void clear_change()
		{
			m_change = false;
		}
	};
}