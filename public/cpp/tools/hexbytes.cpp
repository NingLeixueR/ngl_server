#include "hexbytes.h"

#include <assert.h>

namespace ngl
{
	unsigned char _hexget(unsigned char asource, unsigned char& atarget)
	{
		if (asource >= 'a' && asource <= 'f')
		{
			unsigned char lret = 10;
			atarget = lret + (asource - 'a');
			return true;
		}
		if (asource >= '0' && asource <= '9')
		{
			atarget = (asource - '0');
			return true;
		}
		return false;
	}

	void _bh(unsigned char& apso, unsigned char& apto1, unsigned char& apto2)
	{
		static const char arr[] = { '0','1','2','3','4','5', '6','7','8', '9','a','b', 'c','d','e', 'f' };
		apto1 = arr[apso >> 4];
		apto2 = arr[0x0f & apso];
	}

	bool _hb(unsigned char* apso, unsigned char* apto)
	{
		(*apto) = 0;
		unsigned char ltarget = 0;
		if (_hexget(*apso, ltarget) == false)
			return false;
		(*apto) |= ltarget << 4;
		if (_hexget(*&apso[1], ltarget) == false)
			return false;
		(*apto) |= ltarget;
		return true;
	}

	int hexbytes::to_hex(void* apso, int alen, void* apto)
	{
		int llen = 0;
		unsigned char* lp1;
		unsigned char* lp2;
		for (int i = 0; i < alen; ++i)
		{
			lp1 = (unsigned char*)apso + i;
			lp2 = (unsigned char*)apto + (i * 2);
			_bh(*lp1, *lp2, *(lp2 + 1));
			llen += 2;
		}
		return llen;
	}

	bool hexbytes::to_bytes(void* apso, int alen, void* apto, int& apotlen)
	{
		int llen = 0;
		for (int i = 0; i < alen / 2; ++i)
		{
			if (_hb(&((unsigned char*)apso)[i * 2], &((unsigned char*)apto)[i]) == false)
				return false;
			llen += 2;
		}
		apotlen = alen / 2;
		return true;
	}
}// namespace ngl