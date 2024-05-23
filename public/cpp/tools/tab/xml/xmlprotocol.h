#pragma once

#include <boost/property_tree/ptree.hpp>
#include <functional>
#include <string>
#include <map>

#include "regular.h"
#include "xmlinfo.h"
#include "type.h"
#include "nlog.h"

namespace ngl
{
	class xmlprotocol
	{
		static std::map<std::string, int32_t> m_protocol;

		static void read(const std::string& axmlname);
	public:
		// ### 从xml中加载数据
		static void load();
		// ### 获取协议号
		static int32_t protocol(const std::string& aname);
		// ### makeproto工具调用 所以不需要索引
		static int32_t free_protocol();
	};
}// namespace ngl