#pragma once

#include "manage_csv.h"

namespace ngl
{
	struct ttab_errormessage : 
		public manage_csv<tab_errormessage>
	{
		ttab_errormessage(const ttab_errormessage&) = delete;
		ttab_errormessage& operator=(const ttab_errormessage&) = delete;
		using type_tab = tab_errormessage;

		ttab_errormessage()
		{}

		void reload()final
		{}
	};
}// namespace ngl