// File overview: Implements GoogleTest coverage for bootstrap.

#include <gtest/gtest.h>

#include <string>

#include "runtime_test_support.h"

namespace bootstrap_test_case
{
class TEST_RT_ENV : public ::testing::Environment
{
public:
	void SetUp() override
	{
		std::string lERR;
		if (!ngl_test_support::ENSURE_RT(lERR))
		{
			FAIL() << "test runtime bootstrap failed: " << lERR;
		}
	}
};

::testing::Environment* const g_RT_ENV =
	::testing::AddGlobalTestEnvironment(new TEST_RT_ENV());

TEST(A00Bootstrap, LoadConfigAndCsv)
{
	std::string lERR;
	ASSERT_TRUE(ngl_test_support::ENSURE_RT(lERR)) << lERR;
	ASSERT_NE(ngl::ttab_servers::instance().const_tab(), nullptr);
}

} // namespace bootstrap_test_case
