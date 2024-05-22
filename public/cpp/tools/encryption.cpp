#include "encryption.h"
#include "sysconfig.h"
#include "hexbytes.h"

#include <iostream>
#include <stdio.h>


namespace ngl
{
	void encryption::bytexor(char* ap, int32_t aplen, int apos)
	{
		if (sysconfig::isxor() == false)
			return;
		if (sysconfig::xorkeynum() < 1 || sysconfig::xorkey().empty())
			return;
		for (int i = 0, j = apos; i < aplen; ++i,++j)
		{
			ap[i] = ap[i] ^ sysconfig::xorkey()[j & sysconfig::xorkeynum()];
		}
	}
}// namespace ngl