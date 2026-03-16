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
// File overview: Declares interfaces for logic.

#pragma once

#include <thread>
#include <string>
#include <stack>

namespace ngl
{
	// # remakes
	// In "remakes"string
	// For example:
	//  local_remakes(role, "test_1");
	//  std::string lremakes = local_get_remakes(role); //lremakes == "test_1"
	// {
	//		lremakes = local_get_remakes(role); //lremakes == "test_1"
	//		local_remakes(lremakes, "test_2");
	//		role->addite();
	//		lremakes = local_get_remakes(role); //lremakes == "test_2"
	// }
	//  lremakes = local_get_remakes(role); //lremakes == "test_1"
	class dremakes;
	class remakes
	{
		remakes(const remakes&) = delete;
		remakes& operator=(const remakes&) = delete;

		friend class dremakes;
	private:
		std::stack<std::string> m_remakes;
		void add_remakes(const char* aremakes);
		void erase_remakes();
	public:
		remakes() = default;
		const char* get_remakes();
	};

	class actor_role;

	class dremakes
	{
		dremakes(const dremakes&) = delete;
		dremakes& operator=(const dremakes&) = delete;

		actor_role* m_role = nullptr;
	public:
		explicit dremakes(actor_role* arole, const char* aremakes);
		explicit dremakes(actor_role* arole, const std::string& aremakes);
		virtual ~dremakes();

		static const char* get_remake(actor_role*);
	};
}// namespace ngl

#define local_remakes(ROLE, REMAKES)	ngl::dremakes ldremakes(ROLE, REMAKES)
#define local_get_remakes(ROLE)			ngl::dremakes::get_remake(ROLE)