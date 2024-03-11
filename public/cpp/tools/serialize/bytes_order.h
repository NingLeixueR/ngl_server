#pragma once

#include <cstdint>
#include <bit>

namespace ngl
{
	class bytes_order
	{
	public:
		static constexpr bool islittle();

		static int16_t transformlittle(int16_t avalues);
		static int32_t transformlittle(int32_t& avalues);
		static int64_t transformlittle(int64_t avalues);
	};
}// namespace ngl