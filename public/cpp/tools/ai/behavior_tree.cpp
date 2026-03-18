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
// File overview: Implements logic for ai.

#include "tools/ai/behavior_tree.h"

#include <behaviortree_cpp/action_node.h>

#include <stdexcept>
#include <utility>

namespace ngl
{
	namespace ai
	{
		namespace
		{
			class callback_action_node final :
				public BT::ActionNodeBase
			{
				bt_action_callback m_fun;
			public:
				callback_action_node(const std::string& aname, const BT::NodeConfig& aconfig, bt_action_callback afun) :
					BT::ActionNodeBase(aname, aconfig),
					m_fun(std::move(afun))
				{}

				BT::NodeStatus tick() override
				{
					return m_fun(*this);
				}

				void halt() override
				{
					resetStatus();
				}
			};

			BT::Tree& require_tree(std::unique_ptr<BT::Tree>& atree)
			{
				if (atree == nullptr)
				{
					throw std::runtime_error("behavior_tree is empty");
				}
				return *atree;
			}

			const BT::Tree& require_tree(const std::unique_ptr<BT::Tree>& atree)
			{
				if (atree == nullptr)
				{
					throw std::runtime_error("behavior_tree is empty");
				}
				return *atree;
			}
		}

		behavior_tree::behavior_tree(BT::Tree atree, const BT::Blackboard::Ptr& aroot_blackboard, const BT::Blackboard::Ptr& aglobal_blackboard) :
			m_tree(std::make_unique<BT::Tree>(std::move(atree))),
			m_root_blackboard(aroot_blackboard),
			m_global_blackboard(aglobal_blackboard)
		{}

		bool behavior_tree::empty() const
		{
			return m_tree == nullptr;
		}

		bt_status behavior_tree::tick_once()
		{
			return require_tree(m_tree).tickOnce();
		}

		bt_status behavior_tree::tick_while_running()
		{
			bt_status lstatus = BT::NodeStatus::IDLE;
			do
			{
				lstatus = tick_once();
			} while (lstatus == BT::NodeStatus::RUNNING);
			return lstatus;
		}

		void behavior_tree::halt()
		{
			if (m_tree != nullptr)
			{
				m_tree->haltTree();
			}
		}

		BT::Tree& behavior_tree::native()
		{
			return require_tree(m_tree);
		}

		const BT::Tree& behavior_tree::native() const
		{
			return require_tree(m_tree);
		}

		BT::Blackboard::Ptr behavior_tree::root_blackboard() const
		{
			return m_root_blackboard;
		}

		BT::Blackboard::Ptr behavior_tree::global_blackboard() const
		{
			return m_global_blackboard;
		}

		BT::Blackboard::Ptr behavior_tree_factory::ensure_global_blackboard(const BT::Blackboard::Ptr& aglobal_blackboard)
		{
			return aglobal_blackboard != nullptr ? aglobal_blackboard : BT::Blackboard::create();
		}

		BT::Blackboard::Ptr behavior_tree_factory::make_root_blackboard(const BT::Blackboard::Ptr& aglobal_blackboard)
		{
			return BT::Blackboard::create(ensure_global_blackboard(aglobal_blackboard));
		}

		BT::BehaviorTreeFactory& behavior_tree_factory::native()
		{
			return m_factory;
		}

		const BT::BehaviorTreeFactory& behavior_tree_factory::native() const
		{
			return m_factory;
		}

		void behavior_tree_factory::register_action(const std::string& aid, const bt_action_callback& afun, const bt_ports& aports)
		{
			BT::TreeNodeManifest lmanifest;
			lmanifest.type = BT::NodeType::ACTION;
			lmanifest.registration_ID = aid;
			lmanifest.ports = aports;
			m_factory.registerBuilder(lmanifest,
				[afun](const std::string& aname, const BT::NodeConfig& aconfig)
				{
					return std::make_unique<callback_action_node>(aname, aconfig, afun);
				}
			);
		}

		void behavior_tree_factory::register_condition(const std::string& aid, const bt_condition_callback& afun, const bt_ports& aports)
		{
			m_factory.registerSimpleCondition(aid, afun, aports);
		}

		void behavior_tree_factory::register_tree_from_text(const std::string& axml)
		{
			m_factory.registerBehaviorTreeFromText(axml);
		}

		void behavior_tree_factory::register_tree_from_file(const std::string& apath)
		{
			m_factory.registerBehaviorTreeFromFile(apath);
		}

		behavior_tree behavior_tree_factory::create_tree(const std::string& aid, const BT::Blackboard::Ptr& aglobal_blackboard)
		{
			BT::Blackboard::Ptr lglobal = ensure_global_blackboard(aglobal_blackboard);
			BT::Blackboard::Ptr lroot = make_root_blackboard(lglobal);
			BT::Tree ltree = m_factory.createTree(aid, lroot);
			return behavior_tree(std::move(ltree), lroot, lglobal);
		}

		behavior_tree behavior_tree_factory::create_from_text(const std::string& axml, const BT::Blackboard::Ptr& aglobal_blackboard)
		{
			BT::Blackboard::Ptr lglobal = ensure_global_blackboard(aglobal_blackboard);
			BT::Blackboard::Ptr lroot = make_root_blackboard(lglobal);
			BT::Tree ltree = m_factory.createTreeFromText(axml, lroot);
			return behavior_tree(std::move(ltree), lroot, lglobal);
		}

		behavior_tree behavior_tree_factory::create_from_file(const std::string& apath, const BT::Blackboard::Ptr& aglobal_blackboard)
		{
			BT::Blackboard::Ptr lglobal = ensure_global_blackboard(aglobal_blackboard);
			BT::Blackboard::Ptr lroot = make_root_blackboard(lglobal);
			BT::Tree ltree = m_factory.createTreeFromFile(apath, lroot);
			return behavior_tree(std::move(ltree), lroot, lglobal);
		}
	}// namespace ai
}// namespace ngl
