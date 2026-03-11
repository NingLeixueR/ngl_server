#include <gtest/gtest.h>

#include "tools/nfilterword.h"

namespace
{
void load_default_filter_words()
{
	ngl::nfilterword::clear();
	ngl::nfilterword::init();
	ngl::nfilterword::load("badword");
	ngl::nfilterword::load("123456");
	ngl::nfilterword::build();
}
}

TEST(NFilterWordTest, test_nfilterword)
{
	load_default_filter_words();

	EXPECT_TRUE(ngl::nfilterword::is_filter("this text contains badword"));
	EXPECT_TRUE(ngl::nfilterword::is_filter("b-a-d-w-o-r-d"));
	EXPECT_TRUE(ngl::nfilterword::is_filter("pin: 123456"));
	EXPECT_FALSE(ngl::nfilterword::is_filter("this text is clean"));
}

TEST(NFilterWordTest, OperationsAreSafeWithoutExplicitInit)
{
	ngl::nfilterword::clear();
	ngl::nfilterword::load("unsafe");
	ngl::nfilterword::build();

	EXPECT_TRUE(ngl::nfilterword::is_filter("u-n-s-a-f-e"));
	EXPECT_EQ(ngl::nfilterword::filter("unsafe input"), "****** input");
}

TEST(NFilterWordTest, CheckNameRejectsInvalidUtf8)
{
	const std::string invalid_utf8("\xFF\xFE", 2);
	EXPECT_EQ(
		ngl::nfilterword::check_name(invalid_utf8, 1, 20),
		ngl::nfilterword::enfilter_invalid_utf8
	);
}
