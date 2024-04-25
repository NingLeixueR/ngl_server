#include "bit.h"
#include <cstring>

namespace ngl
{
	template <int N>
	constexpr int _bits()
	{
		return 0x1 << N;
	}

	template <int N>
	constexpr int _invertbits()
	{
		return _bits<N>() ^ 0xff;
	}

	bit::bit()
		:m_bit(nullptr), m_bitarraycount(-1)
	{}

	void bit::init(int8_t* abit, int32_t abitarraycount)
	{
		m_bit = abit;
		m_bitarraycount = abitarraycount;
	}

	bool bit::set(bool abool)
	{
		if (m_bitarraycount < 0)
			return false;
		memset(m_bit, abool ? 0xFF : 0x0, m_bitarraycount);
		return true;
	}

	template <int N>
	void _bit_set(int8_t& avalues, bool abool)
	{
		abool ? avalues |= _bits<N>() : avalues &= _invertbits<N>();
	}

	bool bit::set(int abit, int8_t& avalues, bool abool)
	{
		switch (abit & 7)
		{
		case 0:
			_bit_set<0>(avalues, abool);
			return true;
		case 1:
			_bit_set<1>(avalues, abool);
			return true;
		case 2:
			_bit_set<2>(avalues, abool);
			return true;
		case 3:
			_bit_set<3>(avalues, abool);
			return true;
		case 4:
			_bit_set<4>(avalues, abool);
			return true;
		case 5:
			_bit_set<5>(avalues, abool);
			return true;
		case 6:
			_bit_set<6>(avalues, abool);
			return true;
		case 7:
			_bit_set<7>(avalues, abool);
			return true;
		}
		return false;
	}

	bool bit::set(int abit, bool abool)
	{
		int bit = abit / 8;
		if (bit >= m_bitarraycount)
			return false;
		return set(abit, m_bit[bit], abool);
	}

	bool bit::get(int abit)const
	{
		int bit = abit / 8;
		if (bit > m_bitarraycount || bit < 0)
			return false;
		if (bit == m_bitarraycount)
			return false;
		bool lret = false;
		//switch (abit % (8-1))
		switch (abit & 7)
		{
		case 0:lret = (m_bit[bit] & _bits<0>()) != 0; break;
		case 1:lret = (m_bit[bit] & _bits<1>()) != 0; break;
		case 2:lret = (m_bit[bit] & _bits<2>()) != 0; break;
		case 3:lret = (m_bit[bit] & _bits<3>()) != 0; break;
		case 4:lret = (m_bit[bit] & _bits<4>()) != 0; break;
		case 5:lret = (m_bit[bit] & _bits<5>()) != 0; break;
		case 6:lret = (m_bit[bit] & _bits<6>()) != 0; break;
		case 7:lret = (m_bit[bit] & _bits<7>()) != 0;  break;
		default:
			return false;
		}
		//assert(false);
		return lret;
	}

	int bit::find_first(bool abool)
	{
		for (int i = 0; i < m_bitarraycount; ++i)
		{
			if (abool)
			{
				if (m_bit[i] != 0)
				{
					for (int j = 0; j < 8; ++j)
					{
						if (get(i * 8 + j) == true)
							return i * 8 + j;
					}
				}
			}
			else
			{
				if (m_bit[i] != 0xff)
				{
					for (int j = 0; j < 8; ++j)
					{
						if (get(i * 8 + j) == false)
							return i * 8 + j;
					}
				}
			}
		}
		return -1;
	}
}// namespace ngl