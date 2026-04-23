#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
#include <behaviortree_cpp/bt_factory.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

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

	// Stateful action node wrapper (supports RUNNING status).
	// Used for async operations such as waiting on KCP connections.
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

}// namespace ngl
