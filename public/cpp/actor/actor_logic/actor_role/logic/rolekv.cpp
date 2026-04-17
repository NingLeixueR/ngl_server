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
// File overview: Implements per-role key-value storage operations.


#include "actor/actor_logic/actor_role/logic/rolekv.h"
#include "actor/actor_logic/actor_role/actor_role.h"

namespace ngl
{
	void rolekv::initdata()
	{
		log_error()->print("[db_rolekeyvalue load finish] {}", data());
	}

	const char* rolekv::get_value(const char* akey)
	{
		MODIFIED_RETUAN(lpdbrolekeyvalue, get(get_actor()->id_guid()), nullptr);
		return (*lpdbrolekeyvalue->mutable_mdata())[akey].c_str();
	}

	void rolekv::set_value(const char* akey, const char* adata)
	{
		MODIFIED_RETUAN(lpdbrolekeyvalue, get(get_actor()->id_guid()));
		(*lpdbrolekeyvalue->mutable_mdata())[akey] = adata;
	}
}//namespace ngl