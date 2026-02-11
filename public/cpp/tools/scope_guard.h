/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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