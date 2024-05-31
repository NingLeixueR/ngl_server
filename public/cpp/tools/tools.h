#pragma once

#include <unordered_map>
#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <cctype>
#include <format>
#include <string>
#include <map>

#include <google/protobuf/util/json_util.h>

namespace ngl
{
	static const std::string m_true1 = "true";
	static const std::string m_true2 = "TRUE";

	template <typename Target>
	struct lexical_cast2
	{

	};

	template <>
	struct lexical_cast2<std::string>
	{
		template <typename Source>
		static std::string fun(const Source& source)
		{
			return std::move(std::format("{}", source));
		}

		static std::string fun(const std::string& source)
		{
			return source;
		}

		static std::string fun(const char* source)
		{
			return source;
		}
	};

	template <>
	struct lexical_cast2<int32_t>
	{
		static int32_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static int32_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint32_t>
	{
		static uint32_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static uint32_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int16_t>
	{
		static int16_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static int16_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint16_t>
	{
		static uint16_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static uint16_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int8_t>
	{
		static int8_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static int8_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint8_t>
	{
		static uint8_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static uint8_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int64_t>
	{
		static int64_t fun(const std::string& source)
		{
			return atoll(source.c_str());
		}

		static int64_t fun(const char* source)
		{
			return atoll(source);
		}
	};

	template <>
	struct lexical_cast2<uint64_t>
	{
		static uint64_t fun(const std::string& source)
		{
			return atoll(source.c_str());
		}

		static uint64_t fun(const char* source)
		{
			return atoll(source);
		}
	};

	template <>
	struct lexical_cast2<float>
	{
		static float fun(const std::string& source)
		{
			return (float)std::atof(source.c_str());
		}

		static float fun(const char* source)
		{
			return (float)std::atof(source);
		}
	};


	template <>
	struct lexical_cast2<double>
	{
		static double fun(const std::string& source)
		{
			return std::atof(source.c_str());
		}

		static double fun(const char* source)
		{
			return std::atof(source);
		}
	};


	template <>
	struct lexical_cast2<bool>
	{
		static bool fun(const std::string& source)
		{
			if (source == m_true1 || source == m_true2)
			{
				return true;
			}
			return false;
		}

		static bool fun(const char* source)
		{
			std::string lstr(source);
			return (lstr == "0" || lstr.empty()) ? false : true;
		}

		static bool fun(int64_t source)
		{
			return source != 0;
		}
	};

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

		template <typename TKEY, typename TVAL>
		static void copy(const std::map<TKEY, TVAL>& asource, google::protobuf::Map<TKEY, TVAL>& atarget)
		{
			for (const std::pair<const TKEY, TVAL>& item : asource)
			{
				atarget.insert({ item.first, item.second });
			}
		}

		template <typename TKEY, typename TVAL>
		static void copy(const google::protobuf::Map<TKEY, TVAL>& asource, std::map<TKEY, TVAL>& atarget)
		{
			for (const google::protobuf::MapPair<const TKEY, TVAL>& item : asource)
			{
				atarget.insert({ item.first, item.second });
			}
		}
		
		template <typename To, typename From>
		static To lexical_cast(const From& from)
		{
			return lexical_cast2<To>::fun(from);
		}

		template <typename To>
		static To& lexical_cast(To& from)
		{
			return from;
		}
	};
}//namespace ngl

#define dtype_name(TYPE_) typeid(TYPE_).name()
