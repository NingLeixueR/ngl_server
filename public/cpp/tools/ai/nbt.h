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
	class nbt_context;

	using nbt_status = BT::NodeStatus;
	using nbt_node_callback = std::function<nbt_status(nbt_context&, BT::TreeNode&)>;

	// 全局共享的行为树工厂
	// 节点类型在进程启动时注册一次，所有 actor 实例共享
	class nbt_factory
	{
		std::shared_ptr<BT::BehaviorTreeFactory> m_factory;
		//std::unordered_map<std::string, std::string> m_tree_xml;

		nbt_factory();
	public:
		nbt_factory(const nbt_factory&) = delete;
		nbt_factory& operator=(const nbt_factory&) = delete;

		static nbt_factory& instance();
		static std::shared_ptr<nbt_factory> instance_shared();

		BT::BehaviorTreeFactory& native();

		// 注册 action 节点
		void register_action(
			const std::string& aname,
			nbt_node_callback afun,
			const BT::PortsList& aports = {}
		);

		// 注册 condition 节点
		void register_condition(
			const std::string& aname,
			nbt_node_callback afun,
			const BT::PortsList& aports = {}
		);

		// 注册树 XML
		void register_tree(const std::string& aid, const std::string& axml);
		void register_tree_file(const std::string& apath);

		// 创建树实例（由 nbt_context 内部调用）
		std::unique_ptr<BT::Tree> create(const std::string& aid, BT::Blackboard::Ptr ablackboard);
		std::unique_ptr<BT::Tree> create_xml(const std::string& axml, BT::Blackboard::Ptr ablackboard);
	};

	// 行为树执行上下文，每个 actor 实例持有一个
	// 是行为树与 actor 之间的桥梁
	class nbt_context
	{
		actor*						m_actor = nullptr;
		BT::Blackboard::Ptr			m_blackboard;
		std::unique_ptr<BT::Tree>	m_tree;
		nbt_factory*				m_factory = nullptr;
	public:
		nbt_context() = default;
		~nbt_context();

		nbt_context(const nbt_context&) = delete;
		nbt_context& operator=(const nbt_context&) = delete;
		nbt_context(nbt_context&&) noexcept;
		nbt_context& operator=(nbt_context&&) noexcept;

		// 绑定 actor 和工厂
		void init(actor* aactor, nbt_factory* afactory = nullptr);

		// 从已注册的树 ID 加载
		bool load_tree(const std::string& atreeid);

		// 从 XML 字符串加载
		bool load_tree_xml(const std::string& axml);

		// tick 一次
		nbt_status tick();

		// 中止当前树
		void halt();

		// 树是否为空
		bool empty() const;

		// blackboard 写入
		template <typename T>
		void set(const std::string& akey, const T& aval)
		{
			if (m_blackboard)
			{
				m_blackboard->set(akey, aval);
			}
		}

		// blackboard 读取
		template <typename T>
		T get(const std::string& akey) const
		{
			return m_blackboard->get<T>(akey);
		}

		// 获取绑定的 actor
		template <typename TACTOR>
		TACTOR* get_actor()
		{
			return dynamic_cast<TACTOR*>(m_actor);
		}
		int64_t get_actorid();


		// 获取原始 blackboard
		BT::Blackboard::Ptr blackboard();
	};
}// namespace ngl
