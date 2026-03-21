// File overview: Implements GoogleTest coverage for runtime.

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "runtime_helpers.h"
#include "tools/arg_options.h"
#include "tools/tools.h"

namespace runtime_test_case
{
char g_PROG[] = "ngl_test";
char g_ROBOT[] = "robot";
char g_AREA[] = "1";
char g_TCOUNT[] = "1";
char g_ACCOUNT[] = "alice";
char g_BEGIN[] = "10";
char g_COUNT[] = "20";
TEST(RuntimeHelpersTest, RobotLaunchRequestSupportsInteractiveAndBatchModes)
{
	char* lINTER_ARGV[] = { g_PROG, g_ROBOT, g_AREA, g_TCOUNT };
	const auto lINTER = ngl_runtime::build_robot_req(4, lINTER_ARGV);
	EXPECT_EQ(lINTER.mode, ngl_runtime::robot_launch_mode::interactive);
	EXPECT_TRUE(lINTER.command.empty());

	char* lLOGIN_ARGV[] = { g_PROG, g_ROBOT, g_AREA, g_TCOUNT, g_ACCOUNT };
	const auto lLOGIN = ngl_runtime::build_robot_req(5, lLOGIN_ARGV);
	EXPECT_EQ(lLOGIN.mode, ngl_runtime::robot_launch_mode::login);
	EXPECT_EQ(lLOGIN.command, "login alice");

	char* lLOGINS_ARGV[] = { g_PROG, g_ROBOT, g_AREA, g_TCOUNT, g_ACCOUNT, g_BEGIN, g_COUNT };
	const auto lLOGINS = ngl_runtime::build_robot_req(7, lLOGINS_ARGV);
	EXPECT_EQ(lLOGINS.mode, ngl_runtime::robot_launch_mode::logins);
	EXPECT_EQ(lLOGINS.command, "logins alice 10 20");
}

TEST(RuntimeHelpersTest, RobotLaunchRequestRejectsPartialBatchArgs)
{
	char* lARGV[] = { g_PROG, g_ROBOT, g_AREA, g_TCOUNT, g_ACCOUNT, g_BEGIN };
	const auto lREQ = ngl_runtime::build_robot_req(6, lARGV);
	EXPECT_EQ(lREQ.mode, ngl_runtime::robot_launch_mode::invalid);
	EXPECT_TRUE(lREQ.command.empty());
}

TEST(RuntimeHelpersTest, SplitCommandLineNormalizesRepeatedSpaces)
{
	const std::vector<std::string> lTOKENS = ngl::arg_options::split_command_line("test   1500   login alice");
	ASSERT_EQ(lTOKENS.size(), 4u);
	EXPECT_EQ(lTOKENS[0], "test");
	EXPECT_EQ(lTOKENS[1], "1500");
	EXPECT_EQ(lTOKENS[2], "login");
	EXPECT_EQ(lTOKENS[3], "alice");
}

TEST(RuntimeHelpersTest, PushServerConfigParamEncodesStructuredNetPayload)
{
	ngl::tab_servers lSERVER{};
	lSERVER.m_id = 42;
	lSERVER.m_name = "gateway alpha";
	lSERVER.m_area = 3;
	lSERVER.m_type = ngl::GATEWAY;

	ngl::net_works lTCP{};
	lTCP.m_type = ngl::ENET_TCP;
	lTCP.m_ip = "127.0.0.1";
	lTCP.m_nip = "public.example.com";
	lTCP.m_port = 9000;
	lSERVER.m_net.push_back(lTCP);

	std::string lPARAM;
	ASSERT_TRUE(ngl_runtime::build_push_cfg(lSERVER, lPARAM));
	EXPECT_NE(lPARAM.find("id=42"), std::string::npos);
	EXPECT_NE(lPARAM.find("name=gateway%20alpha"), std::string::npos);

	const std::size_t lNET_POS = lPARAM.find("net=");
	ASSERT_NE(lNET_POS, std::string::npos);
	const std::string lNET = ngl::tools::url_decode(lPARAM.substr(lNET_POS + 4));
	EXPECT_NE(lNET.find("\"tcp\""), std::string::npos);
	EXPECT_NE(lNET.find("127.0.0.1"), std::string::npos);
	EXPECT_NE(lNET.find("public.example.com"), std::string::npos);
}

TEST(RuntimeHelpersTest, PushServerConfigParamRejectsUnknownNetworkType)
{
	ngl::tab_servers lSERVER{};
	lSERVER.m_id = 7;
	lSERVER.m_name = "broken";
	lSERVER.m_area = 1;
	lSERVER.m_type = ngl::GATEWAY;

	ngl::net_works lBAD{};
	lBAD.m_type = static_cast<ngl::ENET_PROTOCOL>(ngl::ENET_COUNT);
	lBAD.m_ip = "127.0.0.1";
	lBAD.m_port = 8000;
	lSERVER.m_net.push_back(lBAD);

	std::string lPARAM = "preset";
	EXPECT_FALSE(ngl_runtime::build_push_cfg(lSERVER, lPARAM));
	EXPECT_TRUE(lPARAM.empty());
}

} // namespace runtime_test_case
