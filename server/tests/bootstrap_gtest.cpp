#include <gtest/gtest.h>

#include <filesystem>
#include <format>
#include <mutex>
#include <string>
#include <vector>

#include "actor/tab/ttab_servers.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"

namespace
{
struct RuntimePaths
{
	const char* config_root;
	const char* csv_root;
};

bool file_exists(const std::filesystem::path& path)
{
	std::error_code ec;
	return std::filesystem::is_regular_file(path, ec);
}

bool candidate_usable(const RuntimePaths& candidate)
{
	namespace fs = std::filesystem;
	std::error_code ec;
	if (!fs::is_directory(candidate.config_root, ec) || !fs::is_directory(candidate.csv_root, ec))
	{
		return false;
	}

	const fs::path config_root(candidate.config_root);
	const fs::path csv_root(candidate.csv_root);
	const bool has_config =
		file_exists(config_root / "config.xml") ||
		file_exists(config_root / "config" / "config.xml");
	const bool has_servers_csv = file_exists(csv_root / "tab_servers.csv");
	return has_config && has_servers_csv;
}

bool resolve_runtime_paths(std::string& config_root, std::string& csv_root, std::string& err)
{
	namespace fs = std::filesystem;

	static const std::vector<RuntimePaths> kCandidates = {
		{ "./config", "./csv" },
		{ "./bin/Debug/config", "./bin/Debug/csv" },
		{ "./bin/Release/config", "./bin/Release/csv" },
		{ "./bin/RelWithDebInfo/config", "./bin/RelWithDebInfo/csv" },
		{ "./bin/MinSizeRel/config", "./bin/MinSizeRel/csv" },
		{ "./bin/configure/config", "./bin/configure/csv" }
	};

	for (const RuntimePaths& candidate : kCandidates)
	{
		if (candidate_usable(candidate))
		{
			config_root = candidate.config_root;
			csv_root = candidate.csv_root;
			return true;
		}
	}

	std::error_code ec;
	const fs::path cwd = fs::current_path(ec);
	const std::string cwd_text = ec ? "<unknown>" : cwd.string();
	err = std::format("unable to locate config/csv directories from cwd={}", cwd_text);
	return false;
}

bool init_test_runtime(std::string& err)
{
	constexpr const char* kNodeName = "db";
	constexpr int kArea = 1;
	constexpr int kTcount = 1;

	std::string config_root;
	std::string csv_root;
	if (!resolve_runtime_paths(config_root, csv_root, err))
	{
		return false;
	}

	nconfig.init();
	if (!nconfig.set_server(kNodeName))
	{
		err = "nconfig.set_server(db) failed";
		return false;
	}

	if (!nconfig.load(config_root, std::format("{}_{}", kNodeName, kTcount)))
	{
		err = std::format("nconfig.load failed: {}", nconfig.config_file());
		return false;
	}

	ngl::csv_base::set_path(csv_root, kNodeName);
	const ngl::tab_servers* tab = ngl::ttab_servers::instance().const_tab(kNodeName, kArea);
	if (tab == nullptr)
	{
		err = "ttab_servers::const_tab(db,1) returned nullptr";
		return false;
	}

	nconfig.set_nodeid(tab->m_id, kTcount);
	nconfig.set_servername(std::format("node_{}_{}_{}", kNodeName, tab->m_area, kTcount));
	return true;
}

bool ensure_runtime_ready(std::string& err)
{
	static std::once_flag once;
	static bool ok = false;
	static std::string cached_err;
	std::call_once(once, []() {
		std::string local_err;
		ok = init_test_runtime(local_err);
		cached_err = local_err;
	});
	if (!ok)
	{
		err = cached_err;
		return false;
	}
	return true;
}

class TestRuntimeEnvironment : public ::testing::Environment
{
public:
	void SetUp() override
	{
		std::string err;
		if (!ensure_runtime_ready(err))
		{
			FAIL() << "test runtime bootstrap failed: " << err;
		}
	}
};

::testing::Environment* const kTestRuntimeEnv =
	::testing::AddGlobalTestEnvironment(new TestRuntimeEnvironment());
}

TEST(A00Bootstrap, LoadConfigAndCsv)
{
	std::string err;
	ASSERT_TRUE(ensure_runtime_ready(err)) << err;
	ASSERT_NE(ngl::ttab_servers::instance().const_tab(), nullptr);
}
