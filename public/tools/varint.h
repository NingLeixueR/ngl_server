#pragma once

#include <cstdint>

namespace ngl
{
	class varint
	{
	public:
		static void		set(bool aisopen);
		
		template <typename T>
		struct parm_length
		{
			T m_value;
		};

		static int length(parm_length<int32_t>& avaluearrays);
		static int length(parm_length<int64_t>& avaluearrays);

		template <typename T>
		struct parm
		{
			T m_value;
			char* m_buf = nullptr;
			int m_len = 0;
			int32_t* m_bytes = 0;
		};

		static bool encode(parm<int64_t>& aparm);
		static bool	decode(parm<int64_t>& aparm);
		static bool	encode(parm<int32_t>& aparm);
		static bool	decode(parm<int32_t>& aparm);		
	};


	void test_varint();
}