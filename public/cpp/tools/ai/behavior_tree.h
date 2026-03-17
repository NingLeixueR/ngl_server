/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for ai.

#pragma once

#include <behaviortree_cpp/bt_factory.h>

#include <functional>
#include <memory>
#include <string>

namespace ngl
{
	namespace ai
	{
		using bt_status = BT::NodeStatus;
		using bt_ports = BT::PortsList;
		using bt_action_callback = std::function<bt_status(BT::TreeNode&)>;
		using bt_condition_callback = std::function<bt_status(BT::TreeNode&)>;

		// Thin project-level wrapper around BT::Tree that keeps both the root
		// blackboard and a process-owned global blackboard alive. The global
		// blackboard can be referenced from XML with the `@name` syntax.
		class behavior_tree
		{
			std::unique_ptr<BT::Tree> m_tree;
			BT::Blackboard::Ptr m_root_blackboard = nullptr;
			BT::Blackboard::Ptr m_global_blackboard = nullptr;
		public:
			behavior_tree() = default;
			behavior_tree(BT::Tree atree, const BT::Blackboard::Ptr& aroot_blackboard, const BT::Blackboard::Ptr& aglobal_blackboard);

			behavior_tree(const behavior_tree&) = delete;
			behavior_tree& operator=(const behavior_tree&) = delete;
			behavior_tree(behavior_tree&&) noexcept = default;
			behavior_tree& operator=(behavior_tree&&) noexcept = default;
			~behavior_tree() = default;

			bool empty() const;

			// Tick the tree exactly once and return the resulting status.
			bt_status tick_once();

			// Convenience loop that keeps ticking until the tree is no longer running.
			bt_status tick_while_running();

			// Halt the entire tree immediately.
			void halt();

			BT::Tree& native();
			const BT::Tree& native() const;

			BT::Blackboard::Ptr root_blackboard() const;
			BT::Blackboard::Ptr global_blackboard() const;
		};

		// Project-level wrapper around BT::BehaviorTreeFactory. It standardizes
		// tree creation so every runtime tree gets a root blackboard chained to an
		// optional global blackboard.
		class behavior_tree_factory
		{
			BT::BehaviorTreeFactory m_factory;

			static BT::Blackboard::Ptr ensure_global_blackboard(const BT::Blackboard::Ptr& aglobal_blackboard);
			static BT::Blackboard::Ptr make_root_blackboard(const BT::Blackboard::Ptr& aglobal_blackboard);
		public:
			behavior_tree_factory() = default;
			behavior_tree_factory(const behavior_tree_factory&) = delete;
			behavior_tree_factory& operator=(const behavior_tree_factory&) = delete;
			behavior_tree_factory(behavior_tree_factory&&) noexcept = default;
			behavior_tree_factory& operator=(behavior_tree_factory&&) noexcept = default;
			~behavior_tree_factory() = default;

			BT::BehaviorTreeFactory& native();
			const BT::BehaviorTreeFactory& native() const;

			void register_action(const std::string& aid, const bt_action_callback& afun, const bt_ports& aports = {});
			void register_condition(const std::string& aid, const bt_condition_callback& afun, const bt_ports& aports = {});

			void register_tree_from_text(const std::string& axml);
			void register_tree_from_file(const std::string& apath);

			behavior_tree create_tree(const std::string& aid, const BT::Blackboard::Ptr& aglobal_blackboard = nullptr);
			behavior_tree create_from_text(const std::string& axml, const BT::Blackboard::Ptr& aglobal_blackboard = nullptr);
			behavior_tree create_from_file(const std::string& apath, const BT::Blackboard::Ptr& aglobal_blackboard = nullptr);
		};
	}// namespace ai
}// namespace ngl
