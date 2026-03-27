// File overview: Implements GoogleTest coverage for nfilterword.

#include <gtest/gtest.h>

#include "tools/tools/tools_filterword.h"

namespace nfilterword_test_case
{
void load_default_filter_words()
{
	ngl::tools::filterword::clear();
	ngl::tools::filterword::init();
	ngl::tools::filterword::load("badword");
	ngl::tools::filterword::load("123456");
	ngl::tools::filterword::build();
}
TEST(NFilterWordTest, test_nfilterword)
{
	load_default_filter_words();

	EXPECT_TRUE(ngl::tools::filterword::is_filter("this text contains badword"));
	EXPECT_TRUE(ngl::tools::filterword::is_filter("b-a-d-w-o-r-d"));
	EXPECT_TRUE(ngl::tools::filterword::is_filter("b@a$d?w=o+r[d]"));
	EXPECT_TRUE(ngl::tools::filterword::is_filter("pin: 123456"));
	EXPECT_FALSE(ngl::tools::filterword::is_filter("this text is clean"));
}

TEST(NFilterWordTest, MatchPreservesSuffixOutputs)
{
	ngl::tools::filterword::clear();
	ngl::tools::filterword::init();
	ngl::tools::filterword::load("abc");
	ngl::tools::filterword::load("bc");
	ngl::tools::filterword::build();

	const auto matches = ngl::tools::filterword::match("zabc");
	ASSERT_EQ(matches.size(), 2U);
	EXPECT_EQ(matches[0].first, 1);
	EXPECT_EQ(matches[0].second, 3);
	EXPECT_EQ(matches[1].first, 2);
	EXPECT_EQ(matches[1].second, 2);
}

TEST(NFilterWordTest, OperationsAreSafeWithoutExplicitInit)
{
	ngl::tools::filterword::clear();
	ngl::tools::filterword::load("unsafe");
	ngl::tools::filterword::build();

	EXPECT_TRUE(ngl::tools::filterword::is_filter("u-n-s-a-f-e"));
	EXPECT_EQ(ngl::tools::filterword::filter("unsafe input"), "****** input");
}

TEST(NFilterWordTest, CheckNameRejectsInvalidUtf8)
{
	const std::string invalid_utf8("\xFF\xFE", 2);
	EXPECT_EQ(
		ngl::tools::filterword::check_name(invalid_utf8, 1, 20),
		ngl::tools::filterword::enfilter_invalid_utf8
	);
}

} // namespace nfilterword_test_case
