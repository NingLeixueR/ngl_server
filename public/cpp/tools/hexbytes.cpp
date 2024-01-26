#include "hexbytes.h"
#include <assert.h>

namespace ngl
{
	unsigned char _hexget(unsigned char avalues)
	{
		if (avalues >= 'a' && avalues <= 'f')
		{
			unsigned char lret = 10;
			return lret + (avalues - 'a');
		}

		if (avalues >= '0' && avalues <= '9')
			return (avalues - '0');

		assert(false);
	}

	void _bh(unsigned char& apso, unsigned char& apto1, unsigned char& apto2)
	{
		static const char arr[] = { '0','1','2','3','4','5', '6','7','8', '9','a','b', 'c','d','e', 'f' };
		apto1 = arr[apso >> 4];
		apto2 = arr[0x0f & apso];
	}

	void _hb(unsigned char* apso, unsigned char* apto)
	{
		(*apto) = 0;
		(*apto) |= _hexget(*apso) << 4;
		(*apto) |= _hexget(*&apso[1]);
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

	int hexbytes::to_bytes(void* apso, int alen, void* apto)
	{
		int llen = 0;
		for (int i = 0; i < alen / 2; ++i)
		{
			_hb(&((unsigned char*)apso)[i * 2], &((unsigned char*)apto)[i]);
			llen += 2;
		}
		return alen / 2;
	}
}