#include "bytes_order.h"


namespace ngl
{
	constexpr bool bytes_order::islittle()
	{
		//static int lint = 1;
		//return ((char*)&lint)[0] == 1;
		return std::endian::native == std::endian::little;
	}

	int16_t bytes_order::transformlittle(int16_t avalues)
	{
		if constexpr (islittle())
			return avalues;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues);
		avalues =
			(static_cast<int16_t>(value_p[0]) << 8)
			| static_cast<int16_t>(value_p[1]);
		return avalues;
	}

	int32_t bytes_order::transformlittle(int32_t& avalues)
	{
		if constexpr (islittle())
			return avalues;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues);
		avalues = (static_cast<int32_t>(value_p[0]) << 24)
			| (static_cast<int32_t>(value_p[1]) << 16)
			| (static_cast<int32_t>(value_p[2]) << 8)
			| static_cast<int32_t>(value_p[3]);
		return avalues;
	}

	int64_t bytes_order::transformlittle(int64_t avalues)
	{
		if constexpr (islittle())
			return avalues;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues);
		avalues = (static_cast<int64_t>(value_p[0]) << 56)
			| (static_cast<int64_t>(value_p[1]) << 48)
			| (static_cast<int64_t>(value_p[2]) << 40)
			| (static_cast<int64_t>(value_p[3]) << 32)
			| (static_cast<int64_t>(value_p[4]) << 24)
			| (static_cast<int64_t>(value_p[5]) << 16)
			| (static_cast<int64_t>(value_p[6]) << 8)
			| static_cast<int64_t>(value_p[7]);
		return avalues;
	}
}