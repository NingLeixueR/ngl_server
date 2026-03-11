#include <gtest/gtest.h>

#include <format>
#include <mutex>
#include <string>

#include "actor/tab/ttab_servers.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"

namespace
{
bool init_test_runtime(std::string& err)
{
	constexpr const char* kNodeName = "db";
	constexpr int kArea = 1;
	constexpr int kTcount = 1;
	constexpr const char* kConfigRoot = "./bin/Debug/config";
	constexpr const char* kCsvRoot = "./bin/Debug/csv";

	nconfig.init();
	if (!nconfig.set_server(kNodeName))
	{
		err = "nconfig.set_server(db) failed";
		return false;
	}

	if (!nconfig.load(kConfigRoot, std::format("{}_{}", kNodeName, kTcount)))
	{
		err = std::format("nconfig.load failed: {}", nconfig.config_file());
		return false;
	}

	ngl::csv_base::set_path(kCsvRoot, kNodeName);
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
