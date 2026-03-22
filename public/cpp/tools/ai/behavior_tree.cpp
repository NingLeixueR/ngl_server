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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
#include <behaviortree_cpp/action_node.h>
#include <behaviortree_cpp/control_node.h>
#include <behaviortree_cpp/decorator_node.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <tinyxml2.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
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

			std::string read_text_file(const std::string& apath)
			{
				std::ifstream file(apath, std::ios::binary);
				if (!file.is_open())
				{
					throw std::runtime_error("failed to open behavior tree file: " + apath);
				}
				return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			}

			const tinyxml2::XMLElement* select_behavior_tree(const tinyxml2::XMLDocument& adoc, const std::string& arequested_id, std::string& aresolved_id)
			{
				const tinyxml2::XMLElement* root = adoc.RootElement();
				if (root == nullptr || std::string(root->Name()) != "root")
				{
					throw std::runtime_error("behavior tree XML must have a <root> element");
				}

				const tinyxml2::XMLElement* selected = nullptr;
				if (!arequested_id.empty())
				{
					for (const tinyxml2::XMLElement* tree = root->FirstChildElement("BehaviorTree"); tree != nullptr; tree = tree->NextSiblingElement("BehaviorTree"))
					{
						const char* id = tree->Attribute("ID");
						if (id != nullptr && arequested_id == id)
						{
							selected = tree;
							aresolved_id = id;
							break;
						}
					}
					if (selected == nullptr)
					{
						throw std::runtime_error("can't find a behavior tree with name: " + arequested_id);
					}
					return selected;
				}

				if (const char* main_tree = root->Attribute("main_tree_to_execute"))
				{
					for (const tinyxml2::XMLElement* tree = root->FirstChildElement("BehaviorTree"); tree != nullptr; tree = tree->NextSiblingElement("BehaviorTree"))
					{
						const char* id = tree->Attribute("ID");
						if (id != nullptr && std::string(main_tree) == id)
						{
							aresolved_id = id;
							return tree;
						}
					}
					throw std::runtime_error("main_tree_to_execute does not match any <BehaviorTree ID>");
				}

				int count = 0;
				for (const tinyxml2::XMLElement* tree = root->FirstChildElement("BehaviorTree"); tree != nullptr; tree = tree->NextSiblingElement("BehaviorTree"))
				{
					selected = tree;
					const char* id = tree->Attribute("ID");
					aresolved_id = id != nullptr ? id : "";
					++count;
				}

				if (count != 1 || selected == nullptr)
				{
					throw std::runtime_error("behavior tree XML must contain exactly one <BehaviorTree> unless main_tree_to_execute is set");
				}
				return selected;
			}

			std::string resolve_node_type_id(const tinyxml2::XMLElement& aelement)
			{
				const std::string element_name = aelement.Name();
				if (element_name == "Action" || element_name == "Condition" || element_name == "Control" || element_name == "Decorator")
				{
					const char* id = aelement.Attribute("ID");
					if (id == nullptr)
					{
						throw std::runtime_error("explicit BT node tags require an ID attribute");
					}
					return id;
				}
				return element_name;
			}

			void initialize_blackboard_ports(const BT::TreeNodeManifest& amanifest, const BT::PortsRemapping& aport_remap, const BT::Blackboard::Ptr& ablackboard, const BT::BehaviorTreeFactory& afactory)
			{
				for (const auto& [port_name, port_info] : amanifest.ports)
				{
					auto remap_it = aport_remap.find(port_name);
					if (remap_it == aport_remap.end())
					{
						continue;
					}

					if (auto port_key = BT::TreeNode::getRemappedKey(port_name, remap_it->second))
					{
						const std::string key = static_cast<std::string>(port_key.value());
						if (auto prev_info = ablackboard->entryInfo(key))
						{
							bool type_mismatch = prev_info->isStronglyTyped() &&
								port_info.isStronglyTyped() &&
								prev_info->type() != port_info.type();
							if (type_mismatch && port_info.direction() == BT::PortDirection::INPUT)
							{
								if (afactory.polymorphicCastRegistry().isConvertible(prev_info->type(), port_info.type()))
								{
									type_mismatch = false;
								}
							}
							if (type_mismatch && prev_info->type() != typeid(std::string))
							{
								throw std::runtime_error("behavior tree port type mismatch on blackboard entry: " + key);
							}
						}
						else
						{
							ablackboard->createEntry(key, port_info);
						}
					}
				}
			}

			void build_tree_node(
				BT::BehaviorTreeFactory& afactory,
				BT::Tree& atree,
				const BT::Tree::Subtree::Ptr& asubtree,
				const BT::Blackboard::Ptr& ablackboard,
				const tinyxml2::XMLElement& aelement,
				BT::TreeNode* aparent,
				const std::string& aprefix_path)
			{
				const std::string type_id = resolve_node_type_id(aelement);
				auto manifest_it = afactory.manifests().find(type_id);
				if (manifest_it == afactory.manifests().end())
				{
					throw std::runtime_error("behavior tree node not registered: " + type_id);
				}
				const BT::TreeNodeManifest& manifest = manifest_it->second;

				const char* attr_name = aelement.Attribute("name");
				const std::string instance_name = attr_name != nullptr ? attr_name : type_id;

				BT::PortsRemapping port_remap;
				BT::NonPortAttributes other_attributes;
				for (const tinyxml2::XMLAttribute* attr = aelement.FirstAttribute(); attr != nullptr; attr = attr->Next())
				{
					const std::string attr_key = attr->Name();
					const std::string attr_value = attr->Value();
					if (attr_key == "ID" || attr_key == "name")
					{
						continue;
					}

					if (BT::IsAllowedPortName(attr_key))
					{
						if (manifest.ports.find(attr_key) == manifest.ports.end())
						{
							throw std::runtime_error("behavior tree XML references an unknown port: " + attr_key);
						}
						port_remap[attr_key] = attr_value;
					}
					else if (!BT::IsReservedAttribute(attr_key))
					{
						other_attributes[attr_key] = attr_value;
					}
				}

				initialize_blackboard_ports(manifest, port_remap, ablackboard, afactory);

				BT::NodeConfig config;
				config.blackboard = ablackboard;
				config.path = aprefix_path + instance_name;
				config.uid = atree.getUID();
				config.manifest = &manifest;
				config.other_attributes = std::move(other_attributes);
				if (type_id == instance_name)
				{
					config.path += "::" + std::to_string(config.uid);
				}

				for (const auto& remap_it : port_remap)
				{
					const auto port_it = manifest.ports.find(remap_it.first);
					if (port_it == manifest.ports.end())
					{
						continue;
					}

					const BT::PortDirection direction = port_it->second.direction();
					if (direction != BT::PortDirection::OUTPUT)
					{
						config.input_ports.insert(remap_it);
					}
					if (direction != BT::PortDirection::INPUT)
					{
						config.output_ports.insert(remap_it);
					}
				}

				auto node = afactory.instantiateTreeNode(instance_name, type_id, config);
				BT::TreeNode* current = node.get();
				if (aparent != nullptr)
				{
					if (auto* control_parent = dynamic_cast<BT::ControlNode*>(aparent))
					{
						control_parent->addChild(current);
					}
					else if (auto* decorator_parent = dynamic_cast<BT::DecoratorNode*>(aparent))
					{
						if (decorator_parent->child() != nullptr)
						{
							throw std::runtime_error("decorator node already has a child: " + aparent->name());
						}
						decorator_parent->setChild(current);
					}
					else
					{
						throw std::runtime_error("leaf node cannot have child nodes: " + aparent->name());
					}
				}

				asubtree->nodes.push_back(std::move(node));

				const tinyxml2::XMLElement* first_child = aelement.FirstChildElement();
				if (first_child == nullptr)
				{
					return;
				}

				if (auto* control = dynamic_cast<BT::ControlNode*>(current))
				{
					for (const tinyxml2::XMLElement* child = first_child; child != nullptr; child = child->NextSiblingElement())
					{
						build_tree_node(afactory, atree, asubtree, ablackboard, *child, control, aprefix_path);
					}
					return;
				}

				if (auto* decorator = dynamic_cast<BT::DecoratorNode*>(current))
				{
					if (first_child->NextSiblingElement() != nullptr)
					{
						throw std::runtime_error("decorator node must have exactly one child: " + current->name());
					}
					build_tree_node(afactory, atree, asubtree, ablackboard, *first_child, decorator, aprefix_path);
					return;
				}

				throw std::runtime_error("leaf node must not have child nodes: " + current->name());
			}

			BT::Tree build_tree_from_xml(BT::BehaviorTreeFactory& afactory, const std::string& axml, const std::string& atree_id, const BT::Blackboard::Ptr& ablackboard)
			{
				tinyxml2::XMLDocument doc;
				doc.Parse(axml.c_str(), axml.size());
				if (doc.Error())
				{
					throw std::runtime_error(std::string("error parsing behavior tree XML: ") + doc.ErrorStr());
				}

				std::string resolved_id;
				const tinyxml2::XMLElement* behavior_tree = select_behavior_tree(doc, atree_id, resolved_id);
				const tinyxml2::XMLElement* root_node = behavior_tree->FirstChildElement();
				if (root_node == nullptr)
				{
					throw std::runtime_error("behavior tree XML must contain a root node under <BehaviorTree>");
				}

				BT::Tree tree;
				auto subtree = std::make_shared<BT::Tree::Subtree>();
				subtree->blackboard = ablackboard;
				subtree->tree_ID = resolved_id;
				subtree->instance_name = resolved_id;
				tree.subtrees.push_back(subtree);

				build_tree_node(afactory, tree, subtree, ablackboard, *root_node, nullptr, "");
				tree.initialize();
				return tree;
			}

			void register_tree_xml(const std::string& axml, std::unordered_map<std::string, std::string>& aregistered_tree_xml)
			{
				tinyxml2::XMLDocument doc;
				doc.Parse(axml.c_str(), axml.size());
				if (doc.Error())
				{
					throw std::runtime_error(std::string("error parsing behavior tree XML: ") + doc.ErrorStr());
				}

				const tinyxml2::XMLElement* root = doc.RootElement();
				if (root == nullptr || std::string(root->Name()) != "root")
				{
					throw std::runtime_error("behavior tree XML must have a <root> element");
				}

				bool found_tree = false;
				for (const tinyxml2::XMLElement* tree = root->FirstChildElement("BehaviorTree"); tree != nullptr; tree = tree->NextSiblingElement("BehaviorTree"))
				{
					const char* id = tree->Attribute("ID");
					if (id == nullptr || std::string(id).empty())
					{
						throw std::runtime_error("registered behavior trees must define an ID attribute");
					}
					aregistered_tree_xml[id] = axml;
					found_tree = true;
				}

				if (!found_tree)
				{
					throw std::runtime_error("behavior tree XML did not define any <BehaviorTree> elements");
				}
			}
		}

		behavior_tree::behavior_tree(BT::Tree atree, const BT::Blackboard::Ptr& aroot_blackboard, const BT::Blackboard::Ptr& aglobal_blackboard, std::shared_ptr<BT::BehaviorTreeFactory> afactory_owner) :
			m_tree(std::make_unique<BT::Tree>(std::move(atree))),
			m_root_blackboard(aroot_blackboard),
			m_global_blackboard(aglobal_blackboard),
			m_factory_owner(std::move(afactory_owner))
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
			return *m_factory;
		}

		const BT::BehaviorTreeFactory& behavior_tree_factory::native() const
		{
			return *m_factory;
		}

		void behavior_tree_factory::register_action(const std::string& aid, const bt_action_callback& afun, const bt_ports& aports)
		{
			BT::TreeNodeManifest lmanifest;
			lmanifest.type = BT::NodeType::ACTION;
			lmanifest.registration_ID = aid;
			lmanifest.ports = aports;
			m_factory->registerBuilder(lmanifest,
				[afun](const std::string& aname, const BT::NodeConfig& aconfig)
				{
					return std::make_unique<callback_action_node>(aname, aconfig, afun);
				}
			);
		}

		void behavior_tree_factory::register_condition(const std::string& aid, const bt_condition_callback& afun, const bt_ports& aports)
		{
			m_factory->registerSimpleCondition(aid, afun, aports);
		}

		void behavior_tree_factory::register_tree_from_text(const std::string& axml)
		{
			register_tree_xml(axml, m_registered_tree_xml);
		}

		void behavior_tree_factory::register_tree_from_file(const std::string& apath)
		{
			register_tree_from_text(read_text_file(apath));
		}

		behavior_tree behavior_tree_factory::create_tree(const std::string& aid, const BT::Blackboard::Ptr& aglobal_blackboard)
		{
			auto it = m_registered_tree_xml.find(aid);
			if (it == m_registered_tree_xml.end())
			{
				throw std::runtime_error("behavior tree not registered: " + aid);
			}
			BT::Blackboard::Ptr lglobal = ensure_global_blackboard(aglobal_blackboard);
			BT::Blackboard::Ptr lroot = make_root_blackboard(lglobal);
			lroot->setPolymorphicCastRegistry(m_factory->polymorphicCastRegistryPtr());
			BT::Tree ltree = build_tree_from_xml(*m_factory, it->second, aid, lroot);
			return behavior_tree(std::move(ltree), lroot, lglobal, m_factory);
		}

		behavior_tree behavior_tree_factory::create_from_text(const std::string& axml, const BT::Blackboard::Ptr& aglobal_blackboard)
		{
			BT::Blackboard::Ptr lglobal = ensure_global_blackboard(aglobal_blackboard);
			BT::Blackboard::Ptr lroot = make_root_blackboard(lglobal);
			lroot->setPolymorphicCastRegistry(m_factory->polymorphicCastRegistryPtr());
			BT::Tree ltree = build_tree_from_xml(*m_factory, axml, "", lroot);
			return behavior_tree(std::move(ltree), lroot, lglobal, m_factory);
		}

		behavior_tree behavior_tree_factory::create_from_file(const std::string& apath, const BT::Blackboard::Ptr& aglobal_blackboard)
		{
			return create_from_text(read_text_file(apath), aglobal_blackboard);
		}
	}// namespace ai
}// namespace ngl
