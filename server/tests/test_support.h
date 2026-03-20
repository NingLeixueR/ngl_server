// File overview: Shared test helpers for server GoogleTest cases.

#pragma once

#include <chrono>
#include <filesystem>
#include <future>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace ngl_test_support
{
	class ScopedCurrentPath
	{
	public:
		explicit ScopedCurrentPath(const std::filesystem::path& target)
			: original_(std::filesystem::current_path())
		{
			std::filesystem::current_path(target);
		}

		ScopedCurrentPath(const ScopedCurrentPath&) = delete;
		ScopedCurrentPath& operator=(const ScopedCurrentPath&) = delete;

		~ScopedCurrentPath()
		{
			std::filesystem::current_path(original_);
		}

	private:
		std::filesystem::path original_;
	};

	template <typename TFUN>
	long long benchmark_us(TFUN&& afun)
	{
		const auto start = std::chrono::steady_clock::now();
		afun();
		const auto finish = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
	}

	inline std::filesystem::path make_temp_test_dir(
		const std::string& test_name,
		std::string_view prefix = "ngl_test",
		bool unique_suffix = true)
	{
		std::string dir_name(prefix);
		dir_name.push_back('_');
		dir_name += test_name;
		if (unique_suffix)
		{
			dir_name.push_back('_');
			dir_name += std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		}

		const std::filesystem::path dir = std::filesystem::temp_directory_path() / std::filesystem::path(dir_name);
		std::error_code ec;
		std::filesystem::remove_all(dir, ec);
		std::filesystem::create_directories(dir, ec);
		return dir;
	}

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
