// File overview: Implements GoogleTest coverage for xml sysconfig.

#include <gtest/gtest.h>

#include <limits>
#include <string>
#include <vector>

#include "runtime_test_support.h"
#include "test_support.h"
#include "tools/tab/xml/sysconfig.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools.h"

namespace xml_sysconfig_test_case
{
class RuntimeConfigTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		std::string lerr;
		ASSERT_TRUE(ngl_test_support::reload_rt(lerr)) << lerr;
	}
};

tinyxml2::XMLElement* legacy_get_child(tinyxml2::XMLElement* aele, const char* astr)
{
	if (aele == nullptr || astr == nullptr)
	{
		return nullptr;
	}

	std::vector<std::string> lparts;
	if (!ngl::tools::splite(astr, ".", lparts))
	{
		return nullptr;
	}

	tinyxml2::XMLElement* lcur = aele;
	for (const std::string& lpart : lparts)
	{
		lcur = lcur->FirstChildElement(lpart.c_str());
		if (lcur == nullptr)
		{
			return nullptr;
		}
	}
	return lcur;
}

template <typename TFun>
bool legacy_foreach_named(
	tinyxml2::XMLElement* aele,
	const char* akey,
	const TFun& afun)
{
	if (aele == nullptr || akey == nullptr)
	{
		return false;
	}

	for (tinyxml2::XMLNode* lchild = aele->FirstChildElement(); lchild != nullptr; lchild = lchild->NextSiblingElement())
	{
		tinyxml2::XMLElement* lele = lchild->ToElement();
		if (lele != nullptr && std::string(lele->Name()) == akey)
		{
			if (!afun(lele))
			{
				return false;
			}
		}
	}
	return true;
}

TEST_F(RuntimeConfigTest, BootstrapConfigKeepsDefaultWssValuesWhenPemFieldsAreBlank)
{
	ASSERT_FALSE(nconfig.config_file().empty());
	EXPECT_TRUE(nconfig.wss().m_certificate_chain.empty());
	EXPECT_TRUE(nconfig.wss().m_private_key.empty());
	EXPECT_TRUE(nconfig.wss().m_ca_certificates.empty());
	EXPECT_EQ(nconfig.wss().m_verify_peer, true);
}

TEST(XmlNodeRuntimeTest, XmlPopUsesDefaultsWhenOptionalWssSectionMissing)
{
	static constexpr char kxml[] = R"xml(
<xmlnode>
	<db db="1" ip="127.0.0.1" port="5432" account="postgres" passworld="123456" dbname="lbtest"/>
	<public>
		<data key="logline">1</data>
	</public>
</xmlnode>
)xml";

	tinyxml2::XMLDocument ldoc;
	ASSERT_EQ(ldoc.Parse(kxml), tinyxml2::XML_SUCCESS);

	auto* lroot = ldoc.FirstChildElement("xmlnode");
	ASSERT_NE(lroot, nullptr);

	ngl::xmlnode lcfg;
	ASSERT_TRUE(lcfg.xml_pop(lroot));
	EXPECT_TRUE(lcfg.wss().m_certificate_chain.empty());
	EXPECT_TRUE(lcfg.wss().m_private_key.empty());
	EXPECT_TRUE(lcfg.wss().m_ca_certificates.empty());
	EXPECT_EQ(lcfg.wss().m_verify_peer, true);
}

TEST(XmlNodeEdgeTest, SetNodeIdRejectsOutOfRangeParts)
{
	ngl::xmlnode lcfg;
	lcfg.set_nodeid(static_cast<int>(std::numeric_limits<int16_t>::max()) + 1, 1);

	EXPECT_EQ(lcfg.tid(), -1);
	EXPECT_EQ(lcfg.tcount(), -1);
}

TEST(XmlNodeEdgeTest, AreaReturnsMinusOneWhenTabMissing)
{
	ngl::xmlnode lcfg;
	lcfg.set_nodeid(static_cast<int>(std::numeric_limits<int16_t>::max()) + 1, 1);

	EXPECT_EQ(lcfg.area(), -1);
}

TEST_F(RuntimeConfigTest, CrossDbSettingsRemainAvailableAfterLoad)
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

	const std::string lorig = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::string lenc = lorig;
	std::string lexp = lorig;
	const std::string& lkey = ngl::sysconfig::xorkey();

	ASSERT_TRUE(ngl::sysconfig::isxor());
	ASSERT_FALSE(lkey.empty());

	for (std::size_t li = 0; li < lexp.size(); ++li)
	{
		lexp[li] = lexp[li] ^ lkey[li % lkey.size()];
	}

	ngl::tools::bytexor(lenc.data(), static_cast<int32_t>(lenc.size()), 0);
	EXPECT_EQ(lenc, lexp);
}

TEST(XmlPerfTest, GetChildPathBenchmark)
{
	tinyxml2::XMLDocument ldoc;
	auto* lroot = ngl::xml::set_child(ldoc, "root");
	ASSERT_NE(lroot, nullptr);
	auto* llv1 = ngl::xml::set_child(lroot, "level1");
	auto* llv2 = ngl::xml::set_child(llv1, "level2");
	auto* lval = ngl::xml::set_child(llv2, "value");
	ASSERT_NE(lval, nullptr);
	ASSERT_TRUE(ngl::xml::set(lval, "payload"));

	const int lk_iter = ngl_test_support::scaled_iterations(300000);
	int lsink = 0;

	const long long legacy_us = ngl_test_support::benchmark_us([lroot, lk_iter, &lsink]()
		{
			for (int li = 0; li < lk_iter; ++li)
			{
				auto* lnode = legacy_get_child(lroot, "level1.level2.value");
				if (lnode != nullptr && lnode->GetText() != nullptr)
				{
					lsink += lnode->GetText()[0];
				}
			}
		});

	const long long optimized_us = ngl_test_support::benchmark_us([lroot, lk_iter, &lsink]()
		{
			for (int li = 0; li < lk_iter; ++li)
			{
				auto* lnode = ngl::xml::get_child(lroot, "level1.level2.value");
				if (lnode != nullptr && lnode->GetText() != nullptr)
				{
					lsink += lnode->GetText()[0];
				}
			}
		});

	EXPECT_GT(lsink, 0);
	EXPECT_NE(ngl::xml::get_child(lroot, "level1.level2.value"), nullptr);
	ngl_test_support::print_perf_result("xml_get_child", legacy_us, optimized_us);
}

TEST(XmlPerfTest, ForeachNamedChildrenBenchmark)
{
	tinyxml2::XMLDocument ldoc;
	auto* lroot = ngl::xml::set_child(ldoc, "root");
	ASSERT_NE(lroot, nullptr);

	for (int li = 0; li < 64; ++li)
	{
		ASSERT_NE(ngl::xml::set_child(lroot, "item"), nullptr);
		ASSERT_NE(ngl::xml::set_child(lroot, "other"), nullptr);
	}

	const int lk_iter = ngl_test_support::scaled_iterations(20000);
	int llegacy_cnt = 0;
	int lopt_cnt = 0;

	const long long legacy_us = ngl_test_support::benchmark_us([lroot, lk_iter, &llegacy_cnt]()
		{
			for (int li = 0; li < lk_iter; ++li)
			{
				ASSERT_TRUE(legacy_foreach_named(lroot, "item", [&llegacy_cnt](tinyxml2::XMLElement*)
					{
						++llegacy_cnt;
						return true;
					}));
			}
		});

	const long long optimized_us = ngl_test_support::benchmark_us([lroot, lk_iter, &lopt_cnt]()
		{
			for (int li = 0; li < lk_iter; ++li)
			{
				ASSERT_TRUE(ngl::xml::foreach(lroot, "item", [&lopt_cnt](tinyxml2::XMLElement*)
					{
						++lopt_cnt;
						return true;
					}));
			}
	});

	EXPECT_EQ(llegacy_cnt, lopt_cnt);
	ngl_test_support::print_perf_result("xml_foreach_named", legacy_us, optimized_us);
}

} // namespace xml_sysconfig_test_case
