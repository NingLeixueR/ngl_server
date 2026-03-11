#include <gtest/gtest.h>

#include "server_main.h"

TEST(A01Smoke, StartupInvalidArgs)
{
	char program_name[] = "ngl_test";
	char* argv[] = { program_name };

	const int rc = ngl_main(1, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::invalid_args));
}
