#pragma once
#include <memory>

namespace ngl
{
	template <typename T>
	class impl
	{
		std::unique_ptr<T> m_impl;
	public:
		template <typename ...ARG>
		void make_unique(const ARG&... args)
		{
			m_impl = std::make_unique<T>(args...);
		}

		inline std::unique_ptr<T>& operator()()
		{
			return m_impl;
		}
	};
}// namespace ngl