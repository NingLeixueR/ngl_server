#include "template_arg.h"
#include <string>

namespace ngl
{
	void test_template_arg()
	{
		template_arg<test_arg, bool>::func<float, bool, std::string>(true);
		template_arg<test_arg2>::func<float, bool, std::string>();
	}
}