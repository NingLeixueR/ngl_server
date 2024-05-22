#pragma once

#include <string>

namespace ngl
{
	class encryption
	{
	public:
		// ## 数据简单加密 异或
		static void bytexor(char* ap, int32_t aplen, int apos);
	};
}//namespace ngl