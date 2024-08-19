#pragma once
#include <memory>

namespace ngl
{
	template <typename T>
	class impl
	{
		impl(const impl&) = delete;
		impl& operator=(const impl&) = delete;

		std::unique_ptr<T> m_impl;
	public:
		impl(){}

		template <typename ...ARG>
		inline void make_unique(const ARG&... args)
		{
			m_impl = std::make_unique<T>(args...);
		}

		inline std::unique_ptr<T>& operator()()
		{
			return m_impl;
		}
	};
}// namespace ngl