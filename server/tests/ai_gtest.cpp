// File overview: Implements GoogleTest coverage for ai.

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "tools/ai/behavior_tree.h"

namespace ai_test_case
{
std::filesystem::path make_ai_temp_dir(const std::string& test_name)
{
	const auto suffix = std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	const std::filesystem::path dir =
		std::filesystem::temp_directory_path() / ("ngl_ai_" + test_name + "_" + suffix);
	std::error_code ec;
	std::filesystem::remove_all(dir, ec);
	std::filesystem::create_directories(dir, ec);
	return dir;
}
TEST(AiTest, FactoryCreatesTreeFromTextWithGlobalBlackboard)
{
	ngl::ai::behavior_tree_factory factory;
	factory.register_condition("HasEnergy",
		[](BT::TreeNode& anode)
		{
			const auto energy = anode.getInput<int>("energy");
			if (!energy)
			{
				return BT::NodeStatus::FAILURE;
			}
			return energy.value() > 0 ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
		},
		{
			BT::InputPort<int>("energy"),
		}
	);
	factory.register_action("ConsumeEnergy",
		[](BT::TreeNode& anode)
		{
			const auto energy = anode.getInput<int>("energy");
			if (!energy)
			{
				return BT::NodeStatus::FAILURE;
			}
			if (!anode.setOutput("energy", energy.value() - 1))
			{
				return BT::NodeStatus::FAILURE;
			}
			return BT::NodeStatus::SUCCESS;
		},
		{
			BT::BidirectionalPort<int>("energy"),
		}
	);

	const std::string xml = R"(
<root BTCPP_format="4">
    <BehaviorTree ID="RobotMain">
        <Sequence>
            <HasEnergy energy="{@energy}"/>
            <ConsumeEnergy energy="{@energy}"/>
        </Sequence>
    </BehaviorTree>
</root>)";

	auto global_blackboard = BT::Blackboard::create();
	global_blackboard->set("energy", 2);

	auto tree = factory.create_from_text(xml, global_blackboard);
	ASSERT_FALSE(tree.empty());
	EXPECT_EQ(tree.global_blackboard(), global_blackboard);
	EXPECT_EQ(tree.tick_once(), BT::NodeStatus::SUCCESS);
	EXPECT_EQ(global_blackboard->get<int>("energy"), 1);
}

TEST(AiTest, FactoryRegistersTreeFromFileAndCreatesById)
{
	ngl::ai::behavior_tree_factory factory;
	int call_count = 0;
	factory.register_action("MarkVisited",
		[&call_count](BT::TreeNode&)
		{
			++call_count;
			return BT::NodeStatus::SUCCESS;
		}
	);

	const std::string xml = R"(
<root BTCPP_format="4">
    <BehaviorTree ID="RobotPatrol">
        <Sequence>
            <MarkVisited/>
        </Sequence>
    </BehaviorTree>
</root>)";

	const std::filesystem::path dir = make_ai_temp_dir("register_from_file");
	const std::filesystem::path xml_file = dir / "robot_patrol.xml";
	{
		std::ofstream out(xml_file);
		ASSERT_TRUE(out.is_open());
		out << xml;
	}

	factory.register_tree_from_file(xml_file.string());
	auto tree = factory.create_tree("RobotPatrol");
	ASSERT_FALSE(tree.empty());
	EXPECT_EQ(tree.tick_once(), BT::NodeStatus::SUCCESS);
	EXPECT_EQ(call_count, 1);
}

TEST(AiTest, TickWhileRunningAdvancesUntilTreeCompletes)
{
	ngl::ai::behavior_tree_factory factory;
	int call_count = 0;
	factory.register_action("WaitForSecondTick",
		[&call_count](BT::TreeNode&)
		{
			++call_count;
			return call_count >= 2 ? BT::NodeStatus::SUCCESS : BT::NodeStatus::RUNNING;
		}
	);

	const std::string xml = R"(
<root BTCPP_format="4">
    <BehaviorTree ID="RobotBootstrap">
        <Sequence>
            <WaitForSecondTick/>
        </Sequence>
    </BehaviorTree>
</root>)";

	auto tree = factory.create_from_text(xml);
	ASSERT_FALSE(tree.empty());
	EXPECT_EQ(tree.tick_while_running(), BT::NodeStatus::SUCCESS);
	EXPECT_EQ(call_count, 2);
}

} // namespace ai_test_case
