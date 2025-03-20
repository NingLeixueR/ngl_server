#pragma once

#include "actor_base.h"
#include "json2binarypack.h"
#include "tools.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	template <typename T>
	class j2p
	{
	public:
		static void fun(const std::string& ajson, std::shared_ptr<ngl::pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			ngl::jsontobinarypack<T>(ajson.c_str(), apack, aactorid, arequestactorid);
		}
	};

	
	std::shared_ptr<pack> actor_base::jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		using type_j2pfun = std::function<void(const std::string&, std::shared_ptr<ngl::pack>&, i64_actorid, i64_actorid)>;
		static std::map<std::string, type_j2pfun> lmap;
		if (lmap.empty())
		{
			lmap[apbname] = std::bind_front(&j2p<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>::fun);
		}
		type_j2pfun* lpfun = tools::findmap(lmap, apbname);
		if (lpfun != nullptr)
		{
			std::shared_ptr<ngl::pack> lpack = std::make_shared<ngl::pack>();
			(*lpfun)(ajson, lpack, aactorid, arequestactorid);
			return lpack;
		}
		return nullptr;
	}
}