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