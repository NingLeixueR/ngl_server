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
// File overview: Declares interfaces for tools.

#pragma once

namespace ngl
{
	template <typename F>
	class scope_guard
	{
		scope_guard() = delete;
		scope_guard(const scope_guard&) = delete;
		scope_guard(const scope_guard&&) = delete;

		F m_fun;
	public:
		template <typename Func>
		requires (!std::same_as<std::remove_cvref_t<Func>, scope_guard<F>>)
		explicit scope_guard(Func&& fun) noexcept
			: m_fun(std::forward<Func>(fun))
		{}

		~scope_guard() noexcept
		{
			try
			{
				m_fun();
			}
			catch (...)
			{
			}
		}
	};

	template <typename Func>
	scope_guard(Func&&) -> scope_guard<std::decay_t<Func>>;

}//namespace ngl
