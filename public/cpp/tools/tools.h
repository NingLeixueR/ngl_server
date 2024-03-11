#pragma once

#include <string>
#include <map>
#include <unordered_map>

#include <google/protobuf/util/json_util.h>
#include <boost/type_index.hpp>

namespace ngl
{
	class tools
	{
	public:
		// map操作封装
		template <typename TE, typename T>
		static const T* findmap(const std::map<TE, T>& amap, const TE& aid)
		{
			auto itor = amap.find(aid);
			return itor == amap.end() ? nullptr : &itor->second;
		}

		template <typename TE, typename T>
		static T* findmap(std::map<TE, T>& amap, const TE& aid)
		{
			auto itor = amap.find(aid);
			return itor == amap.end() ? nullptr : &itor->second;
		}

		template <typename TE, typename T>
		static T* findmap(std::unordered_map<TE, T>& amap, const TE& aid)
		{
			auto itor = amap.find(aid);
			return itor == amap.end() ? nullptr : &itor->second;
		}

		// 是否是内网地址
		static bool is_lanip(const std::string& aip);

		static bool is_mem(const char* astr1, const char* astr2);

		template <typename T>
		static bool protostr(T& adata, std::string& json)
		{
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = false;
			options.always_print_primitive_fields = false;
			options.preserve_proto_field_names = false;
			return google::protobuf::util::MessageToJsonString(adata, &json, options).ok();		
		}

		template <typename T>
		static std::string& type_name(std::string& astr)
		{
			astr = boost::typeindex::type_id_with_cvr<T>().pretty_name();
			return astr;
		}
	};
}//namespace ngl