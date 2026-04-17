/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Item synthesis and crafting recipe evaluation.

#pragma once

#include "tools/tab/csv/ncsv.h"

namespace ngl
{
	class actor_role;
	// Item (itemconvert, )
	class synthesis
	{
	public:
		static bool use(actor_role* arole, int32_t aid);
	};
}// namespace ngl