#include "tools/ai2/nbt.h"
#include "actor/actor_base/core/actor.h"

#include <fstream>
#include <stdexcept>

namespace ngl
{
	namespace ai2
	{
		namespace
		{
			const std::string ctx_key = "_nbt_ctx";

			std::string read_file(const std::string& apath)
			{
				std::ifstream lfile(apath, std::ios::binary);
				if (!lfile.is_open())
				{
					throw std::runtime_error("nbt: failed to open file: " + apath);
				}
				return std::string(
					(std::istreambuf_iterator<char>(lfile)),
					std::istreambuf_iterator<char>()
				);
			}

			class nbt_action_node final : public BT::SyncActionNode
			{
				nbt_node_callback m_fun;
			public:
				nbt_action_node(const std::string& aname, const BT::NodeConfig& aconfig,
					nbt_node_callback afun)
					: BT::SyncActionNode(aname, aconfig)
					, m_fun(std::move(afun))
				{}
				nbt_status tick() override
				{
					auto* lctx = config().blackboard->get<nbt_context*>(ctx_key);
					return m_fun(*lctx, *this);
				}
			};

			class nbt_condition_node final : public BT::ConditionNode
			{
				nbt_node_callback m_fun;
			public:
				nbt_condition_node(const std::string& aname, const BT::NodeConfig& aconfig,
					nbt_node_callback afun)
					: BT::ConditionNode(aname, aconfig)
					, m_fun(std::move(afun))
				{}
				nbt_status tick() override
				{
					auto* lctx = config().blackboard->get<nbt_context*>(ctx_key);
					return m_fun(*lctx, *this);
				}
			};
		}

		// ===== nbt_factory =====

		nbt_factory::nbt_factory()
			: m_factory(std::make_shared<BT::BehaviorTreeFactory>())
		{
		}

		nbt_factory& nbt_factory::instance()
		{
			static nbt_factory linstance;
			return linstance;
		}

		std::shared_ptr<nbt_factory> nbt_factory::instance_shared()
		{
			static auto lptr = std::shared_ptr<nbt_factory>(&instance(), [](nbt_factory*) {});
			return lptr;
		}

		BT::BehaviorTreeFactory& nbt_factory::native()
		{
			return *m_factory;
		}

		void nbt_factory::register_action(
			const std::string& aname,
			nbt_node_callback afun,
			const BT::PortsList& aports)
		{
			BT::NodeBuilder lbuilder =
				[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
				-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_action_node>(ainstance_name, aconfig, afun);
			};

			m_factory->registerBuilder(
				BT::TreeNodeManifest{ BT::NodeType::ACTION, aname, aports, {} },
				lbuilder
			);
		}

		void nbt_factory::register_condition(
			const std::string& aname,
			nbt_node_callback afun,
			const BT::PortsList& aports)
		{
			BT::NodeBuilder lbuilder =
				[afun = std::move(afun)](const std::string& ainstance_name, const BT::NodeConfig& aconfig)
				-> std::unique_ptr<BT::TreeNode>
			{
				return std::make_unique<nbt_condition_node>(ainstance_name, aconfig, afun);
			};

			m_factory->registerBuilder(
				BT::TreeNodeManifest{ BT::NodeType::CONDITION, aname, aports, {} },
				lbuilder
			);
		}

		void nbt_factory::register_tree(const std::string& aid, const std::string& axml)
		{
			m_tree_xml[aid] = axml;
			m_factory->registerBehaviorTreeFromText(axml);
		}

		void nbt_factory::register_tree_file(const std::string& apath)
		{
			std::string lxml = read_file(apath);
			m_factory->registerBehaviorTreeFromText(lxml);
			m_tree_xml[apath] = std::move(lxml);
		}

		std::unique_ptr<BT::Tree> nbt_factory::create(const std::string& aid, BT::Blackboard::Ptr ablackboard)
		{
			auto ltree = std::make_unique<BT::Tree>(m_factory->createTree(aid, ablackboard));
			return ltree;
		}

		std::unique_ptr<BT::Tree> nbt_factory::create_xml(const std::string& axml, BT::Blackboard::Ptr ablackboard)
		{
			auto ltree = std::make_unique<BT::Tree>(m_factory->createTreeFromText(axml, ablackboard));
			return ltree;
		}

		// ===== nbt_context =====

		nbt_context::~nbt_context()
		{
			halt();
		}

		nbt_context::nbt_context(nbt_context&& aother) noexcept
			: m_actor(aother.m_actor)
			, m_blackboard(std::move(aother.m_blackboard))
			, m_tree(std::move(aother.m_tree))
			, m_factory(aother.m_factory)
		{
			aother.m_actor = nullptr;
			aother.m_factory = nullptr;
			if (m_blackboard)
			{
				m_blackboard->set(ctx_key, this);
			}
		}

		nbt_context& nbt_context::operator=(nbt_context&& aother) noexcept
		{
			if (this != &aother)
			{
				halt();
				m_actor = aother.m_actor;
				m_blackboard = std::move(aother.m_blackboard);
				m_tree = std::move(aother.m_tree);
				m_factory = aother.m_factory;
				aother.m_actor = nullptr;
				aother.m_factory = nullptr;
				if (m_blackboard)
				{
					m_blackboard->set(ctx_key, this);
				}
			}
			return *this;
		}

		void nbt_context::init(actor* aactor, nbt_factory* afactory)
		{
			m_actor = aactor;
			m_factory = afactory ? afactory : &nbt_factory::instance();
			m_blackboard = BT::Blackboard::create();
			m_blackboard->set(ctx_key, this);
		}

		bool nbt_context::load_tree(const std::string& atreeid)
		{
			if (m_factory == nullptr)
			{
				return false;
			}
			halt();
			m_tree = m_factory->create(atreeid, m_blackboard);
			return m_tree != nullptr;
		}

		bool nbt_context::load_tree_xml(const std::string& axml)
		{
			if (m_factory == nullptr)
			{
				return false;
			}
			halt();
			m_tree = m_factory->create_xml(axml, m_blackboard);
			return m_tree != nullptr;
		}

		nbt_status nbt_context::tick()
		{
			if (!m_tree)
			{
				return BT::NodeStatus::IDLE;
			}
			return m_tree->tickOnce();
		}

		void nbt_context::halt()
		{
			if (m_tree)
			{
				m_tree->haltTree();
				m_tree.reset();
			}
		}

		bool nbt_context::empty() const
		{
			return !m_tree;
		}

		actor* nbt_context::get_actor()
		{
			return m_actor;
		}

		int64_t nbt_context::get_actorid()
		{
			if (m_actor == nullptr)
			{
				return 0;
			}
			return m_actor->id_guid();
		}

		BT::Blackboard::Ptr nbt_context::blackboard()
		{
			return m_blackboard;
		}
	}// namespace ai2
}// namespace ngl
