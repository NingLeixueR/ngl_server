#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/typeof/typeof.hpp>
#include <functional>
#include <string>
#include <map>

#include "xmlinfo.h"
#include "type.h"
#include "regular.h"
#include "xmlnode.h"
#include "nlog.h"

namespace ngl
{
	class xmlprotocol
	{
		static std::map<std::string, int32_t> m_protocol;

		static void read(const std::string& axmlname);
	public:
		// ### ��xml�м�������
		static void load();
		// ### ��ȡЭ���
		static int32_t protocol(const std::string& aname);
		// ### makeproto���ߵ��� ���Բ���Ҫ����
		static int32_t free_protocol();
	};
}