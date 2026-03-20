// File overview: Implements GoogleTest coverage for bootstrap.

#include <gtest/gtest.h>

#include <string>

#include "runtime_test_support.h"

namespace bootstrap_test_case
{
class TestRuntimeEnvironment : public ::testing::Environment
{
public:
	void SetUp() override
	{
		std::string err;
		if (!ngl_test_support::ensure_bootstrap_runtime(err))
		{
			FAIL() << "test runtime bootstrap failed: " << err;
		}
	}
};

::testing::Environment* const kTestRuntimeEnv =
	::testing::AddGlobalTestEnvironment(new TestRuntimeEnvironment());

TEST(A00Bootstrap, LoadConfigAndCsv)
{
	std::string err;
	ASSERT_TRUE(ngl_test_support::ensure_bootstrap_runtime(err)) << err;
	ASSERT_NE(ngl::ttab_servers::instance().const_tab(), nullptr);
}

} // namespace bootstrap_test_case
