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
#pragma once

#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/decorator_node.h>
#include <behaviortree_cpp/control_node.h>
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

namespace ngl
{
	class actor;

	using nbt_status = BT::NodeStatus;

	template <typename TACTOR>
	class nbt_context;

	template <typename TACTOR>
	using nbt_node_callback = std::function<nbt_status(nbt_context<TACTOR>&, BT::TreeNode&)>;

	// Decorator callback receives the child's tick result in addition to context and node.
	template <typename TACTOR>
	using nbt_decorator_callback = std::function<nbt_status(nbt_context<TACTOR>&, nbt_status, BT::TreeNode&)>;

	// Shared behavior tree factory (singleton per actor type).
	// Node types are registered once at startup; all actor instances share the same factory.
	template <typename TACTOR>
	class nbt_factory
	{
		std::shared_ptr<BT::BehaviorTreeFactory> m_factory;

	public:
		nbt_factory()
			: m_factory(std::make_shared<BT::BehaviorTreeFactory>())
		{
		}
		nbt_factory(const nbt_factory&) = delete;
		nbt_factory& operator=(const nbt_factory&) = delete;

		static nbt_factory<TACTOR>& instance()
		{
			static nbt_factory<TACTOR> linstance;
			return linstance;
		}

		BT::BehaviorTreeFactory& native()
		{
			return *m_factory;
		}

		// Register a synchronous action node.
		void register_action(
			const std::string& aname,
			nbt_node_callback<TACTOR> afun,
			const BT::PortsList& aports = {}
		);

		// Register a stateful action node (may return RUNNING).
		void register_stateful_action(
			const std::string& aname,
			nbt_node_callback<TACTOR> afun,
			const BT::PortsList& aports = {}
		);

		// Register a condition node.
		void register_condition(
			const std::string& aname,
			nbt_node_callback<TACTOR> afun,
			const BT::PortsList& aports = {}
		);

		// Register a decorator node (single child, transforms/gates child result).
		void register_decorator(
			const std::string& aname,
			nbt_decorator_callback<TACTOR> afun,
			const BT::PortsList& aports = {}
		);

		// Register a control node (multiple children, determines tick order and flow).
		void register_control(
			const std::string& aname,
			nbt_node_callback<TACTOR> afun,
			const BT::PortsList& aports = {}
		);

		// Register a coroutine action node (can yield via setStatusRunningAndYield()).
		void register_coro_action(
			const std::string& aname,
			nbt_node_callback<TACTOR> afun,
			const BT::PortsList& aports = {}
		);

		// Register a threaded action node (runs tick() on a separate thread).
		void register_threaded_action(
			const std::string& aname,
			nbt_node_callback<TACTOR> afun,
			const BT::PortsList& aports = {}
		);

		// Register a behavior tree from an XML string.
		void register_tree(const std::string& axml)
		{
			m_factory->registerBehaviorTreeFromText(axml);
		}

		void register_tree_file(const std::string& apath)
		{
			m_factory->registerBehaviorTreeFromFile(apath);
		}

		// Create a tree instance (called internally by nbt_context).
		std::unique_ptr<BT::Tree> create(const std::string& aid, BT::Blackboard::Ptr ablackboard)
		{
			auto ltree = std::make_unique<BT::Tree>(m_factory->createTree(aid, ablackboard));
			return ltree;
		}

		std::unique_ptr<BT::Tree> create_xml(const std::string& axml, BT::Blackboard::Ptr ablackboard)
		{
			auto ltree = std::make_unique<BT::Tree>(m_factory->createTreeFromText(axml, ablackboard));
			return ltree;
		}
	};

	// Per-actor behavior tree execution context.
	// Bridges the behavior tree and the owning actor instance.
	template <typename TACTOR>
	class nbt_context
	{
		TACTOR*						m_actor = nullptr;
		BT::Blackboard::Ptr			m_blackboard;
		std::unique_ptr<BT::Tree>	m_tree;
		nbt_factory<TACTOR>*		m_factory = nullptr;
	public:
		nbt_context() = default;

		~nbt_context()
		{
			halt();
		}

		nbt_context(const nbt_context&) = delete;

		// Bind actor and factory.
		void init(TACTOR* aactor, nbt_factory<TACTOR>* afactory)
		{
			m_actor = aactor;
			m_factory = afactory;
			m_blackboard = BT::Blackboard::create();
			m_blackboard->set(tools::type_name<TACTOR>(), this);
		}

		// Load a tree by registered tree ID.
		bool load_tree(const std::string& atreeid)
		{
			if (m_factory == nullptr)
			{
				return false;
			}
			halt();
			m_tree = m_factory->create(atreeid, m_blackboard);
			return m_tree != nullptr;
		}

		// Load a tree from an XML string.
		bool load_tree_xml(const std::string& axml)
		{
			if (m_factory == nullptr)
			{
				return false;
			}
			halt();
			m_tree = m_factory->create_xml(axml, m_blackboard);
			return m_tree != nullptr;
		}

		// Tick the tree once.
		nbt_status tick()
		{
			if (!m_tree)
			{
				return BT::NodeStatus::IDLE;
			}
			return m_tree->tickOnce();
		}

		// Halt and reset the current tree.
		void halt()
		{
			if (m_tree)
			{
				m_tree->haltTree();
				m_tree.reset();
			}
		}

		// Check whether a tree is loaded.
		bool empty() const
		{
			return !m_tree;
		}

		// Set a value on the blackboard.
		template <typename T>
		void set(const std::string& akey, const T& aval)
		{
			if (m_blackboard)
			{
				m_blackboard->set(akey, aval);
			}
		}

		// Get a value from the blackboard.
		template <typename T>
		T get(const std::string& akey) const
		{
			return m_blackboard->get<T>(akey);
		}

		// Get the bound actor.
		TACTOR* get_actor()
		{
			return m_actor;
		}

		int64_t get_actorid()
		{
			if (m_actor == nullptr)
			{
				return 0;
			}
			return m_actor->id_guid();
		}

		// Get the underlying blackboard.
		BT::Blackboard::Ptr blackboard()
		{
			return m_blackboard;
		}
	};

	template <typename TACTOR>
	class nbt_action_node final : public BT::SyncActionNode
	{
		nbt_node_callback<TACTOR> m_fun;
	public:
		nbt_action_node(const std::string& aname, const BT::NodeConfig& aconfig,
			nbt_node_callback<TACTOR> afun)
			: BT::SyncActionNode(aname, aconfig)
			, m_fun(std::move(afun))
		{}

		nbt_status tick() override
		{
			auto* lctx = config().blackboard->get<nbt_context<TACTOR>*>(tools::type_name<TACTOR>());
			return m_fun(*lctx, *this);
		}
	};

	// Stateful action node wrapper (state-machine lifecycle).
	// Transitions from IDLE → onStart(); if RUNNING is returned, subsequent ticks
	// call onRunning(). When halted externally, onHalted() is invoked for cleanup.
	// Preferred for request-reply patterns where the action must wait across ticks.
	template <typename TACTOR>
	class nbt_stateful_action_node final : public BT::StatefulActionNode
	{
		nbt_node_callback<TACTOR> m_fun;
	public:
		nbt_stateful_action_node(const std::string& aname, const BT::NodeConfig& aconfig,
			nbt_node_callback<TACTOR> afun)
			: BT::StatefulActionNode(aname, aconfig)
			, m_fun(std::move(afun))
		{}

		nbt_status onStart() override
		{
			auto* lctx = config().blackboard->get<nbt_context<TACTOR>*>(tools::type_name<TACTOR>());
			return m_fun(*lctx, *this);
		}

		nbt_status onRunning() override
		{
			auto* lctx = config().blackboard->get<nbt_context<TACTOR>*>(tools::type_name<TACTOR>());
			return m_fun(*lctx, *this);
		}

		void onHalted() override {}
	};

	template <typename TACTOR>
	class nbt_condition_node final : public BT::ConditionNode
	{
		nbt_node_callback<TACTOR> m_fun;
	public:
		nbt_condition_node(const std::string& aname, const BT::NodeConfig& aconfig,
			nbt_node_callback<TACTOR> afun)
			: BT::ConditionNode(aname, aconfig)
			, m_fun(std::move(afun))
		{}
		nbt_status tick() override
		{
			auto* lctx = config().blackboard->get<nbt_context<TACTOR>*>(tools::type_name<TACTOR>());
			return m_fun(*lctx, *this);
		}
	};

	// Decorator node wrapper (exactly one child).
	// Ticks the child first, then passes the child's result to the callback.
	// The callback decides the final status — e.g. inverting SUCCESS/FAILURE,
	// repeating the child, or adding timeout logic.
	template <typename TACTOR>
	class nbt_decorator_node final : public BT::DecoratorNode
	{
		nbt_decorator_callback<TACTOR> m_fun;
	public:
		nbt_decorator_node(const std::string& aname, const BT::NodeConfig& aconfig,
			nbt_decorator_callback<TACTOR> afun)
			: BT::DecoratorNode(aname, aconfig)
			, m_fun(std::move(afun))
		{}

		nbt_status tick() override
		{
			auto child_status = child()->executeTick();
			auto* lctx = config().blackboard->get<nbt_context<TACTOR>*>(tools::type_name<TACTOR>());
			return m_fun(*lctx, child_status, *this);
		}
	};

	// Control node wrapper (multiple children).
	// Determines the order and conditions under which children are ticked.
	// The callback has full access to children() and haltChildren() via the
	// TreeNode reference, enabling custom flow logic (sequence, fallback, parallel, etc.).
	template <typename TACTOR>
	class nbt_control_node final : public BT::ControlNode
	{
		nbt_node_callback<TACTOR> m_fun;
	public:
		nbt_control_node(const std::string& aname, const BT::NodeConfig& aconfig,
			nbt_node_callback<TACTOR> afun)
			: BT::ControlNode(aname, aconfig)
			, m_fun(std::move(afun))
		{}

		nbt_status tick() override
		{
			auto* lctx = config().blackboard->get<nbt_context<TACTOR>*>(tools::type_name<TACTOR>());
			return m_fun(*lctx, *this);
		}
	};

	// Coroutine action node wrapper.
	// Allows the callback to suspend mid-execution by calling
	// setStatusRunningAndYield() on the node, returning RUNNING to the parent.
	// On the next tick the coroutine resumes where it left off.
	template <typename TACTOR>
	class nbt_coro_action_node final : public BT::CoroActionNode
	{
		nbt_node_callback<TACTOR> m_fun;
	public:
		nbt_coro_action_node(const std::string& aname, const BT::NodeConfig& aconfig,
			nbt_node_callback<TACTOR> afun)
			: BT::CoroActionNode(aname, aconfig)
			, m_fun(std::move(afun))
		{}

		nbt_status tick() override
		{
			auto* lctx = config().blackboard->get<nbt_context<TACTOR>*>(tools::type_name<TACTOR>());
			return m_fun(*lctx, *this);
		}
	};

	// Threaded action node wrapper.
	// Runs tick() on a dedicated thread. The callback must periodically check
	// isHaltRequested() on the node and return promptly when true.
	// Suitable for blocking I/O or CPU-intensive work that should not stall the tree.
	template <typename TACTOR>
	class nbt_threaded_action_node final : public BT::ThreadedAction
	{
		nbt_node_callback<TACTOR> m_fun;
	public:
		nbt_threaded_action_node(const std::string& aname, const BT::NodeConfig& aconfig,
			nbt_node_callback<TACTOR> afun)
			: BT::ThreadedAction(aname, aconfig)
			, m_fun(std::move(afun))
		{}

		nbt_status tick() override
		{
			auto* lctx = config().blackboard->get<nbt_context<TACTOR>*>(tools::type_name<TACTOR>());
			return m_fun(*lctx, *this);
		}
	};

	template <typename TACTOR>
	void nbt_factory<TACTOR>::register_action(
		const std::string& aname,
		nbt_node_callback<TACTOR> afun,
		const BT::PortsList& aports /*= {}*/
	)
	{
		BT::NodeBuilder lbuilder =
			[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
			-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_action_node<TACTOR>>(ainstance_name, aconfig, afun);
			};

		m_factory->registerBuilder(
			BT::TreeNodeManifest{ BT::NodeType::ACTION, aname, aports, {} },
			lbuilder
		);
	}

	template <typename TACTOR>
	void nbt_factory<TACTOR>::register_stateful_action(
		const std::string& aname,
		nbt_node_callback<TACTOR> afun,
		const BT::PortsList& aports /*= {}*/
	)
	{
		BT::NodeBuilder lbuilder =
			[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
			-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_stateful_action_node<TACTOR>>(ainstance_name, aconfig, afun);
			};

		m_factory->registerBuilder(
			BT::TreeNodeManifest{ BT::NodeType::ACTION, aname, aports, {} },
			lbuilder
		);
	}

	template <typename TACTOR>
	void nbt_factory<TACTOR>::register_condition(
		const std::string& aname,
		nbt_node_callback<TACTOR> afun,
		const BT::PortsList& aports /*= {}*/
	)
	{
		BT::NodeBuilder lbuilder =
			[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
			-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_condition_node<TACTOR>>(ainstance_name, aconfig, afun);
			};

		m_factory->registerBuilder(
			BT::TreeNodeManifest{ BT::NodeType::CONDITION, aname, aports, {} },
			lbuilder
		);
	}

	template <typename TACTOR>
	void nbt_factory<TACTOR>::register_decorator(
		const std::string& aname,
		nbt_decorator_callback<TACTOR> afun,
		const BT::PortsList& aports /*= {}*/
	)
	{
		BT::NodeBuilder lbuilder =
			[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
			-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_decorator_node<TACTOR>>(ainstance_name, aconfig, afun);
			};

		m_factory->registerBuilder(
			BT::TreeNodeManifest{ BT::NodeType::DECORATOR, aname, aports, {} },
			lbuilder
		);
	}

	template <typename TACTOR>
	void nbt_factory<TACTOR>::register_control(
		const std::string& aname,
		nbt_node_callback<TACTOR> afun,
		const BT::PortsList& aports /*= {}*/
	)
	{
		BT::NodeBuilder lbuilder =
			[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
			-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_control_node<TACTOR>>(ainstance_name, aconfig, afun);
			};

		m_factory->registerBuilder(
			BT::TreeNodeManifest{ BT::NodeType::CONTROL, aname, aports, {} },
			lbuilder
		);
	}

	template <typename TACTOR>
	void nbt_factory<TACTOR>::register_coro_action(
		const std::string& aname,
		nbt_node_callback<TACTOR> afun,
		const BT::PortsList& aports /*= {}*/
	)
	{
		BT::NodeBuilder lbuilder =
			[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
			-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_coro_action_node<TACTOR>>(ainstance_name, aconfig, afun);
			};

		m_factory->registerBuilder(
			BT::TreeNodeManifest{ BT::NodeType::ACTION, aname, aports, {} },
			lbuilder
		);
	}

	template <typename TACTOR>
	void nbt_factory<TACTOR>::register_threaded_action(
		const std::string& aname,
		nbt_node_callback<TACTOR> afun,
		const BT::PortsList& aports /*= {}*/
	)
	{
		BT::NodeBuilder lbuilder =
			[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
			-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_threaded_action_node<TACTOR>>(ainstance_name, aconfig, afun);
			};

		m_factory->registerBuilder(
			BT::TreeNodeManifest{ BT::NodeType::ACTION, aname, aports, {} },
			lbuilder
		);
	}

}// namespace ngl
