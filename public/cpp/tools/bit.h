#pragma once

#include <cstdint>
#include <vector>

namespace ngl
{
	class bit
	{
		int8_t* m_bit;
		int32_t m_bitarraycount;
	public:
		bit();

		void init(int8_t* abit, int32_t abitarraycount);

		void set_all(bool abool);

		static bool set(int abit, int8_t& avalues, bool abool);

		bool set(int abit, bool abool);

		bool get(int abit)const;

		int find_first(bool abool);
	};
}// namespace ngl