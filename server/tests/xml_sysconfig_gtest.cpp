// File overview: Implements GoogleTest coverage for xml sysconfig.

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "tools/tab/xml/sysconfig.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools.h"

namespace xml_sysconfig_test_case
{
tinyxml2::XMLElement* legacy_get_child(tinyxml2::XMLElement* aele, const char* astr)
{
	if (aele == nullptr || astr == nullptr)
	{
		return nullptr;
	}

	std::vector<std::string> parts;
	if (!ngl::tools::splite(astr, ".", parts))
	{
		return nullptr;
	}

	tinyxml2::XMLElement* current = aele;
	for (const std::string& part : parts)
	{
		current = current->FirstChildElement(part.c_str());
		if (current == nullptr)
		{
			return nullptr;
		}
	}
	return current;
}

bool legacy_foreach_named(
	tinyxml2::XMLElement* aele,
	const char* akey,
	const std::function<bool(tinyxml2::XMLElement*)>& afun)
{
	if (aele == nullptr || akey == nullptr)
	{
		return false;
	}

	for (tinyxml2::XMLNode* child = aele->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		tinyxml2::XMLElement* element = child->ToElement();
		if (element != nullptr && std::string(element->Name()) == akey)
		{
			if (!afun(element))
			{
				return false;
			}
		}
	}
	return true;
}

template <typename TFUN>
long long benchmark_us(TFUN&& afun)
{
	const auto start = std::chrono::steady_clock::now();
	afun();
	const auto finish = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
}
TEST(XmlNodeRuntimeTest, BootstrapConfigLoadsConfiguredWssSection)
{
	ASSERT_FALSE(nconfig.config_file().empty());
	EXPECT_FALSE(nconfig.wss().m_certificate_chain.empty());
	EXPECT_FALSE(nconfig.wss().m_private_key.empty());
	EXPECT_FALSE(nconfig.wss().m_ca_certificates.empty());
	EXPECT_EQ(nconfig.wss().m_verify_peer, true);
}

TEST(XmlNodeRuntimeTest, XmlPopUsesDefaultsWhenOptionalWssSectionMissing)
{
	static constexpr char kXml[] = R"xml(
<xmlnode>
	<db db="1" ip="127.0.0.1" port="5432" account="postgres" passworld="123456" dbname="lbtest"/>
	<public>
		<data key="logline">1</data>
	</public>
</xmlnode>
)xml";

	tinyxml2::XMLDocument document;
	ASSERT_EQ(document.Parse(kXml), tinyxml2::XML_SUCCESS);

	auto* root = document.FirstChildElement("xmlnode");
	ASSERT_NE(root, nullptr);

	ngl::xmlnode config;
	ASSERT_TRUE(config.xml_pop(root));
	EXPECT_TRUE(config.wss().m_certificate_chain.empty());
	EXPECT_TRUE(config.wss().m_private_key.empty());
	EXPECT_TRUE(config.wss().m_ca_certificates.empty());
	EXPECT_EQ(config.wss().m_verify_peer, true);
}

TEST(XmlNodeRuntimeTest, CrossDbSettingsRemainAvailableAfterLoad)
{
	ASSERT_FALSE(nconfig.crossdb().m_ip.empty());
	EXPECT_GT(nconfig.crossdb().m_port, 0u);
	EXPECT_FALSE(nconfig.crossdb().m_dbname.empty());
}

TEST(SysconfigRuntimeTest, InitReadsLegacyAliasesAndKeepsWholeXorKey)
{
	ngl::sysconfig::init();

	EXPECT_EQ(ngl::sysconfig::logflushtime(), 10000000);
	ASSERT_TRUE(ngl::sysconfig::isxor());
	ASSERT_FALSE(ngl::sysconfig::xorkey().empty());
	EXPECT_EQ(ngl::sysconfig::xorkeynum(), static_cast<int32_t>(ngl::sysconfig::xorkey().size()));
}

TEST(SysconfigRuntimeTest, ByteXorUsesCompleteConfiguredKeyStream)
{
	ngl::sysconfig::init();

	const std::string original = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::string encrypted = original;
	std::string expected = original;
	const std::string& key = ngl::sysconfig::xorkey();

	ASSERT_TRUE(ngl::sysconfig::isxor());
	ASSERT_FALSE(key.empty());

	for (std::size_t i = 0; i < expected.size(); ++i)
	{
		expected[i] = expected[i] ^ key[i % key.size()];
	}

	ngl::tools::bytexor(encrypted.data(), static_cast<int32_t>(encrypted.size()), 0);
	EXPECT_EQ(encrypted, expected);
}

TEST(XmlPerfTest, GetChildPathBenchmark)
{
	tinyxml2::XMLDocument document;
	auto* root = ngl::xml::set_child(document, "root");
	ASSERT_NE(root, nullptr);
	auto* level1 = ngl::xml::set_child(root, "level1");
	auto* level2 = ngl::xml::set_child(level1, "level2");
	auto* value = ngl::xml::set_child(level2, "value");
	ASSERT_NE(value, nullptr);
	ASSERT_TRUE(ngl::xml::set(value, "payload"));

	constexpr int kIterations = 300000;
	volatile int sink = 0;

	const long long legacy_us = benchmark_us([&]()
		{
			for (int i = 0; i < kIterations; ++i)
			{
				auto* node = legacy_get_child(root, "level1.level2.value");
				if (node != nullptr && node->GetText() != nullptr)
				{
					sink += node->GetText()[0];
				}
			}
		});

	const long long optimized_us = benchmark_us([&]()
		{
			for (int i = 0; i < kIterations; ++i)
			{
				auto* node = ngl::xml::get_child(root, "level1.level2.value");
				if (node != nullptr && node->GetText() != nullptr)
				{
					sink += node->GetText()[0];
				}
			}
		});

	EXPECT_GT(sink, 0);
	EXPECT_NE(ngl::xml::get_child(root, "level1.level2.value"), nullptr);
	std::cout << "[perf] xml_get_child legacy_us=" << legacy_us
		<< " optimized_us=" << optimized_us << std::endl;
}

TEST(XmlPerfTest, ForeachNamedChildrenBenchmark)
{
	tinyxml2::XMLDocument document;
	auto* root = ngl::xml::set_child(document, "root");
	ASSERT_NE(root, nullptr);

	for (int i = 0; i < 64; ++i)
	{
		ASSERT_NE(ngl::xml::set_child(root, "item"), nullptr);
		ASSERT_NE(ngl::xml::set_child(root, "other"), nullptr);
	}

	constexpr int kIterations = 20000;
	volatile int legacy_count = 0;
	volatile int optimized_count = 0;

	const long long legacy_us = benchmark_us([&]()
		{
			for (int i = 0; i < kIterations; ++i)
			{
				ASSERT_TRUE(legacy_foreach_named(root, "item", [&legacy_count](tinyxml2::XMLElement*)
					{
						++legacy_count;
						return true;
					}));
			}
		});

	const long long optimized_us = benchmark_us([&]()
		{
			for (int i = 0; i < kIterations; ++i)
			{
				ASSERT_TRUE(ngl::xml::foreach(root, "item", [&optimized_count](tinyxml2::XMLElement*)
					{
						++optimized_count;
						return true;
					}));
			}
		});

	EXPECT_EQ(legacy_count, optimized_count);
	std::cout << "[perf] xml_foreach_named legacy_us=" << legacy_us
		<< " optimized_us=" << optimized_us << std::endl;
}

} // namespace xml_sysconfig_test_case
