// File overview: Shared test helpers for server GoogleTest cases.

#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace ngl_test_support
{
	inline std::string read_env_value(const char* name)
	{
		if (name == nullptr || *name == '\0')
		{
			return {};
		}

#ifdef _MSC_VER
		char* raw = nullptr;
		std::size_t len = 0;
		if (_dupenv_s(&raw, &len, name) != 0 || raw == nullptr)
		{
			return {};
		}

		std::string value(raw);
		std::free(raw);
		return value;
#else
		const char* raw = std::getenv(name);
		return raw == nullptr ? std::string() : std::string(raw);
#endif
	}

	inline bool env_flag_enabled(const char* name)
	{
		const std::string value_storage = read_env_value(name);
		if (value_storage.empty())
		{
			return false;
		}

		const std::string_view value(value_storage);
		return value == "1" || value == "true" || value == "TRUE" || value == "yes" || value == "on";
	}

	inline bool test_verbose_enabled()
	{
		static const bool enabled = env_flag_enabled("NGL_TEST_VERBOSE");
		return enabled;
	}

	inline int perf_scale_percent()
	{
		static const int percent = []()
		{
			const std::string value = read_env_value("NGL_TEST_PERF_SCALE");
			if (value.empty())
			{
				return 100;
			}

			char* end = nullptr;
			const long parsed = std::strtol(value.c_str(), &end, 10);
			if (end == value.c_str() || parsed <= 0)
			{
				return 100;
			}
			return static_cast<int>(std::clamp(parsed, 1L, 1000L));
		}();
		return percent;
	}

	inline int scaled_iterations(int base_iterations)
	{
		if (base_iterations <= 0)
		{
			return 1;
		}

		const long long scaled =
			(static_cast<long long>(base_iterations) * perf_scale_percent() + 99LL) / 100LL;
		return static_cast<int>(std::max<long long>(1, scaled));
	}

	inline void trace(std::string_view message)
	{
		if (test_verbose_enabled())
		{
			std::cout << message << std::endl;
		}
	}

	inline void print_perf_result(std::string_view name, long long legacy_us, long long optimized_us)
	{
		if (test_verbose_enabled())
		{
			std::cout << std::format(
				"[perf] {} scale={} legacy_us={} optimized_us={}",
				name,
				perf_scale_percent(),
				legacy_us,
				optimized_us)
				<< std::endl;
		}
	}

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
