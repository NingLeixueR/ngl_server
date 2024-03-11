#pragma once

#include <cstdint>

namespace ngl
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Varint是一种使用一个或多个字节序列化整数的方法会把整数编码为变长字节。
	// 对于32位整型数据经过Varint编码后需要1~5个字节，小的数字使用1个byte，大的数字使用5个bytes。
	// 64位整型数据编码后占用1~10个字节。
	// 在实际场景中小数字的使用率远远多于大数字，因此通过Varint编码对于大部分场景都可以起到很好的压缩效果。
	
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
}