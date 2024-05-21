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

		static bool set(int abit, int8_t& avalues, bool abool);

		//# 将所有位都设置为abool
		bool set(bool abool);

		//# 把abit位设置为abool
		bool set(int abit, bool abool);

		//# 获取abit位的bool
		bool get(int abit)const;

		//# 查找首个出现abool值的位置
		int find_first(bool abool);
	};
}// namespace ngl