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

	inline bool env_flag(const char* aname)
	{
		const std::string lvalue = read_env(aname);
		if (lvalue.empty())
		{
			return false;
		}

		const std::string_view lview(lvalue);
		return lview == "1" || lview == "true" || lview == "TRUE" || lview == "yes" || lview == "on";
	}

	inline bool test_verbose()
	{
		static const bool g_test_verbose = env_flag("NGL_TEST_VERBOSE");
		return g_test_verbose;
	}

	inline int perf_scale()
	{
		static const int g_perf_scale = []()
		{
			const std::string lvalue = read_env("NGL_TEST_PERF_SCALE");
			if (lvalue.empty())
			{
				return 100;
			}

			char* lend = nullptr;
			const long lparsed = std::strtol(lvalue.c_str(), &lend, 10);
			if (lend == lvalue.c_str() || lparsed <= 0)
			{
				return 100;
			}
			return static_cast<int>(std::clamp(lparsed, 1L, 1000L));
		}();
		return g_perf_scale;
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

	class scoped_path
	{
	public:
		explicit scoped_path(const std::filesystem::path& atarget)
			: m_ori(std::filesystem::current_path())
		{
			std::filesystem::current_path(atarget);
		}

		scoped_path(const scoped_path&) = delete;
		scoped_path& operator=(const scoped_path&) = delete;

		~scoped_path()
		{
			std::filesystem::current_path(m_ori);
		}

	private:
		std::filesystem::path m_ori;
	};

	template <typename tfun>
	long long benchmark_us(tfun&& afun)
	{
		const auto start = std::chrono::steady_clock::now();
		afun();
		const auto finish = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
	}

	inline std::filesystem::path make_tmp_dir(
		const std::string& atest_name,
		std::string_view aprefix = "ngl_test",
		bool aunique_suffix = true)
	{
		std::string ldir_name(aprefix);
		ldir_name.push_back('_');
		ldir_name += atest_name;
		if (aunique_suffix)
		{
			ldir_name.push_back('_');
			ldir_name += std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		}

		const std::filesystem::path ldir =
			std::filesystem::temp_directory_path() / std::filesystem::path(ldir_name);
		std::error_code lec;
		std::filesystem::remove_all(ldir, lec);
		std::filesystem::create_directories(ldir, lec);
		return ldir;
	}

	template <typename tval>
	void try_set(const std::shared_ptr<std::promise<tval>>& apromise, tval avalue)
	{
		if (apromise == nullptr)
		{
			return;
		}
		try
		{
			apromise->set_value(std::move(avalue));
		}
		catch (const std::future_error&)
		{
		}
	}

	inline void try_set(const std::shared_ptr<std::promise<void>>& apromise)
	{
		if (apromise == nullptr)
		{
			return;
		}
		try
		{
			apromise->set_value();
		}
		catch (const std::future_error&)
		{
		}
	}
}
