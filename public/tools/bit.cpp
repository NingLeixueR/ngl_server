#include "bit.h"

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

	void bit::set_all(bool abool)
	{
		memset(m_bit, abool ? 0xFF : 0x0, m_bitarraycount);
	}

	void bit::vectorbooltobytes(const std::vector<bool>& avec, std::vector<int8_t>& aintvec)
	{
		for (int i = 0; i < avec.size(); ++i)
		{
			if ((i & 7) == 0)
				aintvec.push_back(0x0);
			set(i, *aintvec.rbegin(), avec[i]);
		}
	}

	void bit::bytestovectorbool(const std::vector<int8_t>& avec, std::vector<bool>& aboolvec)
	{
		if (avec.empty())
			return;
		bit lbit;
		lbit.init((int8_t*)&avec[0], avec.size());
		int lsize = avec.size() * 8;
		for (int i = 0; i < lsize; ++i)
		{
			aboolvec.push_back(lbit.get(i));
		}
	}

	bool bit::set(int abit, int8_t& avalues, bool abool)
	{
		switch (abit & 7)
		{
		case 0:abool ? avalues |= _bits<0>() : avalues &= _invertbits<0>(); return true;
		case 1:abool ? avalues |= _bits<1>() : avalues &= _invertbits<1>(); return true;
		case 2:abool ? avalues |= _bits<2>() : avalues &= _invertbits<2>(); return true;
		case 3:abool ? avalues |= _bits<3>() : avalues &= _invertbits<3>(); return true;
		case 4:abool ? avalues |= _bits<4>() : avalues &= _invertbits<4>(); return true;
		case 5:abool ? avalues |= _bits<5>() : avalues &= _invertbits<5>(); return true;
		case 6:abool ? avalues |= _bits<6>() : avalues &= _invertbits<6>(); return true;
		case 7:abool ? avalues |= _bits<7>() : avalues &= _invertbits<7>(); return true;
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
}