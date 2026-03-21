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
	inline std::string read_env(const char* name)
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

	inline bool env_flag(const char* name)
	{
		const std::string lVALUE = read_env(name);
		if (lVALUE.empty())
		{
			return false;
		}

		const std::string_view lVIEW(lVALUE);
		return lVIEW == "1" || lVIEW == "true" || lVIEW == "TRUE" || lVIEW == "yes" || lVIEW == "on";
	}

	inline bool test_verbose()
	{
		static const bool g_TEST_VERBOSE = env_flag("NGL_TEST_VERBOSE");
		return g_TEST_VERBOSE;
	}

	inline int perf_scale()
	{
		static const int g_PERF_SCALE = []()
		{
			const std::string lVALUE = read_env("NGL_TEST_PERF_SCALE");
			if (lVALUE.empty())
			{
				return 100;
			}

			char* lEND = nullptr;
			const long lPARSED = std::strtol(lVALUE.c_str(), &lEND, 10);
			if (lEND == lVALUE.c_str() || lPARSED <= 0)
			{
				return 100;
			}
			return static_cast<int>(std::clamp(lPARSED, 1L, 1000L));
		}();
		return g_PERF_SCALE;
	}

	inline int scaled_iterations(int base_iterations)
	{
		if (base_iterations <= 0)
		{
			return 1;
		}

		const long long scaled =
			(static_cast<long long>(base_iterations) * perf_scale() + 99LL) / 100LL;
		return static_cast<int>(std::max<long long>(1, scaled));
	}

	inline void trace(std::string_view message)
	{
		if (test_verbose())
		{
			std::cout << message << std::endl;
		}
	}

	inline void print_perf_result(std::string_view name, long long legacy_us, long long optimized_us)
	{
		if (test_verbose())
		{
			std::cout << std::format(
				"[perf] {} scale={} legacy_us={} optimized_us={}",
				name,
				perf_scale(),
				legacy_us,
				optimized_us)
				<< std::endl;
		}
	}

	class ScopedPath
	{
	public:
		explicit ScopedPath(const std::filesystem::path& aTARGET)
			: m_ORI(std::filesystem::current_path())
		{
			std::filesystem::current_path(aTARGET);
		}

		ScopedPath(const ScopedPath&) = delete;
		ScopedPath& operator=(const ScopedPath&) = delete;

		~ScopedPath()
		{
			std::filesystem::current_path(m_ORI);
		}

	private:
		std::filesystem::path m_ORI;
	};

	template <typename TFUN>
	long long benchmark_us(TFUN&& afun)
	{
		const auto start = std::chrono::steady_clock::now();
		afun();
		const auto finish = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
	}

	inline std::filesystem::path make_tmp_dir(
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
	void try_set(const std::shared_ptr<std::promise<T>>& aPROMISE, T aVALUE)
	{
		if (aPROMISE == nullptr)
		{
			return;
		}
		try
		{
			aPROMISE->set_value(std::move(aVALUE));
		}
		catch (const std::future_error&)
		{
		}
	}

	inline void try_set(const std::shared_ptr<std::promise<void>>& aPROMISE)
	{
		if (aPROMISE == nullptr)
		{
			return;
		}
		try
		{
			aPROMISE->set_value();
		}
		catch (const std::future_error&)
		{
		}
	}
}
