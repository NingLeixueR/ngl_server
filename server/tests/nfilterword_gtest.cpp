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
