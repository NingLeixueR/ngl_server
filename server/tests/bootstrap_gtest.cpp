// File overview: Implements GoogleTest coverage for bootstrap.

#include <gtest/gtest.h>

#include <string>

#include "runtime_test_support.h"

namespace bootstrap_test_case
{
class test_rt_env : public ::testing::Environment
{
public:
	void SetUp() override
	{
		std::string lerr;
		if (!ngl_test_support::ensure_rt(lerr))
		{
			FAIL() << "test runtime bootstrap failed: " << lerr;
		}
	}
};

[[maybe_unused]] const ::testing::Environment* const g_rt_env =
	::testing::AddGlobalTestEnvironment(new test_rt_env());

TEST(A00Bootstrap, LoadConfigAndCsv)
{
	std::string lerr;
	ASSERT_TRUE(ngl_test_support::ensure_rt(lerr)) << lerr;
	ASSERT_NE(ngl::ttab_servers::instance().const_tab(), nullptr);
}

} // namespace bootstrap_test_case
