// File overview: Shared test helpers for server GoogleTest cases.

#pragma once

#include <future>
#include <memory>
#include <utility>

namespace ngl_test_support
{
	template <typename T>
	void try_set_promise_value(const std::shared_ptr<std::promise<T>>& promise, T value)
	{
		try
		{
			promise->set_value(std::move(value));
		}
		catch (const std::future_error&)
		{
		}
	}

	inline void try_set_promise_value(const std::shared_ptr<std::promise<void>>& promise)
	{
		try
		{
			promise->set_value();
		}
		catch (const std::future_error&)
		{
		}
	}
}
