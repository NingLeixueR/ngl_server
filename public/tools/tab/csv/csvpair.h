#pragma once

#include <string>

namespace ngl
{
	struct csvpair
	{
		std::string m_data;
		int m_pos;
		bool m_doublequotationmarks; // Ë«ÒýºÅ
		char m_fg;//Ä¬ÈÏ,

		csvpair();
		void clear();
	};
}
