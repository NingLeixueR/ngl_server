/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "lua.hpp"

#include <google/protobuf/map.h>
#include <google/protobuf/repeated_field.h>

#include <vector>
#include <string>
#include <map>
#include <set>

//# define LOG_SCRIPT(...)  ::ngl::log_error()->print(__VA_ARGS__)
# define LOG_SCRIPT(...)  std::cout << std::format(__VA_ARGS__) << std::endl

namespace ngl
{
	class luaapi
	{
	public:
		static bool isnumber(lua_State* L);

		static void pushnumber(lua_State* L, double adata);

		static bool popnumber(lua_State* L, double& adata, bool apop = true);

		static bool isnil(lua_State* L);

		static void pushnil(lua_State* L);

		static bool popnil(lua_State* L, bool apop = true);

		static bool isinteger(lua_State* L);

		static void pushinteger(lua_State* L, int64_t aval);

		static bool popinteger(lua_State* L, int64_t& aval, bool apop = true);

		static bool isstring(lua_State* L);

		static const char* pushstring(lua_State* L, const char* astr);

		static const char* pushstring(lua_State* L, const std::string& astr);

		static bool popstring(lua_State* L, std::string& aval, bool apop = true);

		static bool popstring(lua_State* L, const char*& aval, bool apop = true);

		static bool isboolean(lua_State* L);

		static void pushboolean(lua_State* L, bool aval);

		static bool popboolean(lua_State* L, bool& aval, bool apop = true);

		static void register_func(lua_State* L, const char* afuncname, lua_CFunction fn);

		static void add_package_path(lua_State* L, const char* apath);
	};


	template <typename T>
	struct serialize_lua
	{
		static void stack_push(lua_State* L, const T& adata);

		static bool stack_pop(lua_State* L, T& adata, bool apop = true);

		static void table_push(lua_State* L, const char* aname, const T& adata);

		static bool table_pop(lua_State* L, const char* aname, T& adata);
	};

	template <typename T>
	struct serialize_lua<T*>
	{
		static void stack_push(lua_State* L, const T* adata)
		{
			serialize_lua<T>::stack_push(L, *adata);
		}

		static bool stack_pop(lua_State* L, T* adata, bool apop = true)
		{
			serialize_lua<T>::stack_pop(L, *adata, apop);
		}

		static void table_push(lua_State* L, const char* aname, const T* adata)
		{
			serialize_lua<T>::table_push(L, aname, *adata);
		}

		static bool table_pop(lua_State* L, const char* aname, T* adata)
		{
			serialize_lua<T>::table_pop(L, aname, *adata);
		}
	};

	template <>
	struct serialize_lua<double>
	{
		static void stack_push(lua_State* L, double adata)
		{
			luaapi::pushnumber(L, adata);
		}

		static bool stack_pop(lua_State* L, double& adata, bool apop = true)
		{
			return luaapi::popnumber(L, adata, apop);
		}

		static void table_push(lua_State* L, const char* aname, double adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, double& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<float>
	{
		static void stack_push(lua_State* L, float adata)
		{
			luaapi::pushnumber(L, adata);
		}

		static bool stack_pop(lua_State* L, float& adata, bool apop = true)
		{
			double lvalue = 0.0;
			if (luaapi::popnumber(L, lvalue, apop))
			{
				adata = (float)lvalue;
				return true;
			}
			return false;
		}

		static void table_push(lua_State* L, const char* aname, float adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, float& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<bool>
	{
		static void stack_push(lua_State* L, bool adata)
		{
			luaapi::pushboolean(L, adata);
		}

		static bool stack_pop(lua_State* L, bool& adata, bool apop = true)
		{
			return luaapi::popboolean(L, adata, apop);
		}

		static void table_push(lua_State* L, const char* aname, bool adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, bool& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<std::string>
	{
		static void stack_push(lua_State* L, const std::string& adata)
		{
			luaapi::pushstring(L, adata);
		}

		static bool stack_pop(lua_State* L, std::string& adata, bool apop = true)
		{
			return luaapi::popstring(L, adata, apop);
		}

		static void table_push(lua_State* L, const char* aname, const std::string& adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, std::string& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<const char*>
	{
		static void stack_push(lua_State* L, const char* adata)
		{
			luaapi::pushstring(L, adata);
		}

		static bool stack_pop(lua_State* L, const char*& adata, bool apop = true)
		{
			return luaapi::popstring(L, adata, apop);
		}

		static void table_push(lua_State* L, const char* aname, const char* adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, const char*& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<int8_t>
	{
		static void stack_push(lua_State* L, int8_t adata)
		{
			luaapi::pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, int8_t& adata, bool apop = true)
		{
			int64_t lvalue = 0;
			if (luaapi::popinteger(L, lvalue, apop))
			{
				adata = (int8_t)lvalue;
				return true;
			}
			return false;
		}

		static void table_push(lua_State* L, const char* aname, int8_t adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, int8_t& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<int16_t>
	{
		static void stack_push(lua_State* L, int16_t adata)
		{
			luaapi::pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, int16_t& adata, bool apop = true)
		{
			int64_t lvalue = 0;
			if (luaapi::popinteger(L, lvalue, apop))
			{
				adata = (int16_t)lvalue;
				return true;
			}
			return false;
		}

		static void table_push(lua_State* L, const char* aname, int16_t adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, int16_t& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<int32_t>
	{
		static void stack_push(lua_State* L, int32_t adata)
		{
			luaapi::pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, int32_t& adata, bool apop = true)
		{
			int64_t lvalue = 0;
			if (luaapi::popinteger(L, lvalue, apop))
			{
				adata = (int32_t)lvalue;
				return true;
			}
			return false;
		}

		static void table_push(lua_State* L, const char* aname, int32_t adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, int32_t& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<int64_t>
	{
		static void stack_push(lua_State* L, int64_t adata)
		{
			luaapi::pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, int64_t& adata, bool apop = true)
		{
			return luaapi::popinteger(L, adata, apop);
		}

		static void table_push(lua_State* L, const char* aname, int64_t adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, int64_t& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<uint8_t>
	{
		static void stack_push(lua_State* L, uint8_t adata)
		{
			luaapi::pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, uint8_t& adata, bool apop = true)
		{
			int64_t lvalue = 0;
			if (luaapi::popinteger(L, lvalue, apop))
			{
				adata = (uint8_t)lvalue;
				return true;
			}
			return false;
		}

		static void table_push(lua_State* L, const char* aname, uint8_t adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, uint8_t& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<uint16_t>
	{
		static void stack_push(lua_State* L, uint16_t adata)
		{
			luaapi::pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, uint16_t& adata, bool apop = true)
		{
			int64_t lvalue = 0;
			if (luaapi::popinteger(L, lvalue, apop))
			{
				adata = (uint16_t)lvalue;
				return true;
			}
			return false;
		}

		static void table_push(lua_State* L, const char* aname, uint16_t adata)
		{
			lua_pushinteger(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, uint16_t& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<uint32_t>
	{
		static void stack_push(lua_State* L, uint32_t adata)
		{
			luaapi::pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, uint32_t& adata, bool apop = true)
		{
			int64_t lvalue = 0;
			if (luaapi::popinteger(L, lvalue, apop))
			{
				adata = (uint32_t)lvalue;
				return true;
			}
			return false;
		}

		static void table_push(lua_State* L, const char* aname, uint32_t adata)
		{
			lua_pushinteger(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, uint32_t& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<uint64_t>
	{
		static void stack_push(lua_State* L, uint64_t adata)
		{
			luaapi::pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, uint64_t& adata, bool apop = true)
		{
			int64_t lvalue = 0;
			if (luaapi::popinteger(L, lvalue, apop))
			{
				adata = (uint64_t)lvalue;
				return true;
			}
			return false;
		}

		static void table_push(lua_State* L, const char* aname, uint64_t adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool table_pop(lua_State* L, const char* aname, uint64_t& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	struct serialize_lua_map
	{
		template <typename KEY, typename VAL>
		static bool keyvalues(lua_State* L, google::protobuf::Map<KEY, VAL>& amap)
		{
			if (!lua_istable(L, -1))
			{
				return false;
			}

			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				KEY lkey;
				VAL lvalue;
				// 弹出value，保留key用于下一次迭代
				if (!(serialize_lua<VAL>::stack_pop(L, lvalue) && serialize_lua<KEY>::stack_pop(L, lkey, false)))
				{
					return false;
				}
				amap[lkey] = lvalue;
			}
			return true;
		}

		template <typename KEY, typename VAL>
		static bool keyvalues(lua_State* L, std::map<KEY, VAL>& amap)
		{
			if (!lua_istable(L, -1))
			{
				return false;
			}

			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				KEY lkey;
				VAL lvalue;
				// 弹出value，保留key用于下一次迭代
				if (!(serialize_lua<VAL>::stack_pop(L, lvalue) && serialize_lua<KEY>::stack_pop(L, lkey, false)))
				{
					return false;
				}
				amap[lkey] = lvalue;
			}
			return true;
		}
	};

	template <typename T>
	struct serialize_lua<std::vector<T>>
	{
		static void stack_push(lua_State* L, const std::vector<T>& adata)
		{
			lua_newtable(L);
			for (size_t i = 0; i < adata.size(); ++i)
			{
				serialize_lua<T>::stack_push(L, adata[i]);
				lua_rawseti(L, -2, i + 1);
			}
		}

		static void table_push(lua_State* L, const char* aname, const std::vector<T>& adata)
		{
			stack_push(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool stack_pop(lua_State* L, std::vector<T>& adata, bool apop = true)
		{
			if (!lua_isnil(L, -1))
			{
				std::map<int32_t, T> lmap;
				if (!serialize_lua_map::keyvalues(L, lmap))
				{
					return false;
				}
				for (const auto& item : lmap)
				{
					adata.push_back(item.second);
				}
			}
			
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, std::vector<T>& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <typename T>
	struct serialize_lua<std::set<T>>
	{
		static void stack_push(lua_State* L, const std::set<T>& adata)
		{
			lua_newtable(L);
			int32_t i = 0;
			for (const T& item : adata)
			{
				serialize_lua<T>::stack_push(L, item);
				lua_rawseti(L, -2, ++i);
			}
		}

		static void table_push(lua_State* L, const char* aname, const std::set<T>& adata)
		{
			stack_push(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool stack_pop(lua_State* L, std::set<T>& adata, bool apop = true)
		{
			if (!lua_isnil(L, -1))
			{
				std::map<int32_t, T> lmap;
				if (!serialize_lua_map::keyvalues(L, lmap))
				{
					return false;
				}
				for (const auto& item : lmap)
				{
					adata.insert(item.second);
				}
			}
			
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, std::set<T>& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	// 只有各种int 与 string
	template <typename T>
	struct serialize_lua_table_key
	{
		static void key(lua_State* L, T adata)
		{
			lua_rawseti(L, -2, adata);
		}
	};

	template <>
	struct serialize_lua_table_key<std::string>
	{
		static void key(lua_State* L, const std::string& adata)
		{
			lua_setfield(L, -2, adata.c_str());
		}
	};

	template <typename KEY, typename VAL>
	struct serialize_lua<std::map<KEY, VAL>>
	{
		static void stack_push(lua_State* L, const std::map<KEY, VAL>& adata)
		{
			lua_newtable(L);
			for (const auto& item : adata)
			{
				serialize_lua<VAL>::table_push(L, nullptr, item.second);
				serialize_lua_table_key<KEY>::key(L, item.first);
			}
		}

		static void table_push(lua_State* L, const char* aname, const std::map<KEY, VAL>& adata)
		{
			stack_push(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool stack_pop(lua_State* L, std::map<KEY, VAL>& adata, bool apop = true)
		{
			if (!lua_isnil(L, -1))
			{
				if (!serialize_lua_map::keyvalues(L, adata))
				{
					return false;
				}
			}
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, std::map<KEY, VAL>& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <typename KEY, typename VAL>
	struct serialize_lua<google::protobuf::Map<KEY, VAL>>
	{
		static void stack_push(lua_State* L, const google::protobuf::Map<KEY, VAL>& adata)
		{
			lua_newtable(L);
			for (const auto& item : adata)
			{
				serialize_lua<VAL>::table_push(L, nullptr, item.second);
				serialize_lua_table_key<KEY>::key(L, item.first);
			}
		}

		static void table_push(lua_State* L, const char* aname, const google::protobuf::Map<KEY, VAL>& adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool stack_pop(lua_State* L, google::protobuf::Map<KEY, VAL>& adata, bool apop = true)
		{
			if (!lua_isnil(L, -1))
			{
				if (!serialize_lua_map::keyvalues(L, adata))
				{
					return false;
				}
			}
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, google::protobuf::Map<KEY, VAL>& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <typename T>
	struct serialize_lua<google::protobuf::RepeatedPtrField<T>>
	{
		static void stack_push(lua_State* L, const google::protobuf::RepeatedPtrField<T>& adata)
		{
			lua_newtable(L);
			for (size_t i = 0; i < adata.size(); ++i)
			{
				serialize_lua<T>::stack_push(L, adata[(int32_t)i]);
				lua_rawseti(L, -2, i + 1);
			}
		}

		static void table_push(lua_State* L, const char* aname, const google::protobuf::RepeatedPtrField<T>& adata)
		{
			stack_push(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool stack_pop(lua_State* L, google::protobuf::RepeatedPtrField<T>& adata, bool apop = true)
		{
			if (!lua_isnil(L, -1))
			{
				std::map<int32_t, T> lmap;
				if (!serialize_lua_map::keyvalues(L, lmap))
				{
					return false;
				}
				for (const auto& item : lmap)
				{
					*adata.Add() = item.second;
				}
			}
			
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, google::protobuf::RepeatedPtrField<T>& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	template <typename T>
	struct serialize_lua<google::protobuf::RepeatedField<T>>
	{
		static void stack_push(lua_State* L, const google::protobuf::RepeatedField<T>& adata)
		{
			lua_newtable(L);
			for (size_t i = 0; i < adata.size(); ++i)
			{
				serialize_lua<T>::stack_push(L, adata[(int32_t)i]);
				lua_rawseti(L, -2, i + 1);
			}
		}

		static void table_push(lua_State* L, const char* aname, const google::protobuf::RepeatedField<T>& adata)
		{
			stack_push(L, adata);
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
		}

		static bool stack_pop(lua_State* L, google::protobuf::RepeatedField<T>& adata, bool apop = true)
		{
			if (!lua_isnil(L, -1))
			{
				std::map<int32_t, T> lmap;
				if (!serialize_lua_map::keyvalues(L, lmap))
				{
					return false;
				}
				for (const auto& item : lmap)
				{
					*adata.Add() = item.second;
				}
			}
			
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, google::protobuf::RepeatedField<T>& adata)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return stack_pop(L, adata);
		}
	};

	class nlua_stack
	{
	public:
		static void stack_push(lua_State* L)
		{
		}

		template <typename T>
		static void stack_push(lua_State* L, const T& adata)
		{
			serialize_lua<T>::stack_push(L, adata);
		}

		template <typename T, typename ...TARGS>
		static void stack_push(lua_State* L, const T& adata, const TARGS&... aargs)
		{
			serialize_lua<T>::stack_push(L, adata);
			stack_push(L, aargs...);
		}

		static bool stack_pop(lua_State* L)
		{
			return true;
		}

		template <typename T>
		static bool stack_pop(lua_State* L, T& adata)
		{
			return serialize_lua<T>::stack_pop(L, adata);
		}

		template <typename T, typename ...TARGS>
		static bool stack_pop(lua_State* L, T& adata, TARGS&... aargs)
		{
			return  stack_pop(L, aargs...) && serialize_lua<T>::stack_pop(L, adata);
		}

		template <typename ...TARGS>
		static bool stack_pop(lua_State* L, TARGS&... aargs)
		{
			return stack_pop(L, aargs...);
		}
	};

	struct nlua_tableinfo
	{
		lua_State* L = nullptr;
		const char* m_name = nullptr;
	};

	class nlua_table
	{
	public:
		template <typename T>
		static void table_push(lua_State* L, const char* aname, const T& adata)
		{
			serialize_lua<T>::table_push(L, aname, adata);
		}

		template <typename T, typename ...TARGS>
		static void table_push(lua_State* L, const char* aname, const T& adata, const TARGS&... args)
		{
			serialize_lua<T>::table_push(L, aname, adata);
			table_push(L, args...);
		}

		static void table_push(nlua_tableinfo& ainfo)
		{
		}

		template <typename ...TARGS>
		static void table_push(nlua_tableinfo& ainfo, const TARGS&... args)
		{
			lua_newtable(ainfo.L);
			table_push(ainfo.L, args...);
			if (ainfo.m_name != nullptr)
			{
				lua_setfield(ainfo.L, -2, ainfo.m_name);
			}
		}

		static bool table_start_push(lua_State* L, const char* aname)
		{
			lua_newtable(L);
			return true;
		}

		static bool table_finish_push(lua_State* L, const char* aname)
		{
			if (aname != nullptr)
			{
				lua_setfield(L, -2, aname);
			}
			return true;
		}

		template <typename T>
		static bool table_pop(lua_State* L, const char* aname, T& adata)
		{
			return serialize_lua<T>::table_pop(L, aname, adata);
		}

		template <typename T, typename ...TARGS>
		static bool table_pop(lua_State* L, const char* aname, T& adata, TARGS&... args)
		{
			return table_pop(L, args...) && serialize_lua<T>::table_pop(L, aname, adata);
		}

		static bool table_pop(nlua_tableinfo& ainfo)
		{
			return true;
		}

		template <typename ...TARGS>
		static bool table_pop(nlua_tableinfo& ainfo, TARGS&... args)
		{
			if (ainfo.m_name != nullptr)
			{
				lua_getfield(ainfo.L, -1, ainfo.m_name);
			}
			if (table_isnil(ainfo.L))
			{
				return true;
			}
			return table_pop(ainfo.L, args...);
		}

		static bool table_isnil(lua_State* L)
		{
			if (lua_isnil(L, -1))
			{
				return true;
			}
			return false;
		}

		static bool table_start_pop(lua_State* L, const char* aname)
		{
			if (aname != nullptr)
			{
				lua_getfield(L, -1, aname);
			}
			return true;
		}

		static bool table_finish_pop(lua_State* L, const char* aname)
		{
			return true;
		}
	};


	template <typename T>
	void serialize_lua<T>::stack_push(lua_State* L, const T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			int32_t lvalue = (int32_t)(adata);
			serialize_lua<int32_t>::stack_push(L, lvalue);
		}
		else
		{
			adata.nlua_push(L, nullptr);
		}
	}

	template <typename T>
	bool serialize_lua<T>::stack_pop(lua_State* L, T& adata, bool apop/* = true*/)
	{
		if constexpr (std::is_enum<T>::value)
		{
			int32_t lvalue = 0;
			if (!serialize_lua<int32_t>::stack_pop(L, lvalue))
			{
				return false;
			}
			adata = (T)lvalue;
		}
		else
		{
			if (!lua_isnil(L, -1))
			{
				if (!adata.nlua_pop(L))
				{
					return false;
				}
			}			
		}
		if (apop)
		{
			lua_pop(L, 1);
		}
		return true;
	}

	template <typename T>
	void serialize_lua<T>::table_push(lua_State* L, const char* aname, const T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			stack_push(L, adata);
		}
		else
		{
			adata.nlua_push(L, aname);
		}
	}

	template <typename T>
	bool serialize_lua<T>::table_pop(lua_State* L, const char* aname, T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			return stack_pop(L, adata);
		}
		else
		{
			return adata.nlua_pop(L, aname);
		}
	}

}//namespace ngl

template <bool POP>
class help_nlua
{
	std::vector<const char*>& m_vec;
	int32_t m_pos = 0;
	lua_State* L = nullptr;
	const char* m_name = nullptr;
public:
	help_nlua(lua_State* aL, const char* aname, std::vector<const char*>& avec) :
		m_vec(avec),
		L(aL),
		m_name(aname)
	{
		if constexpr (POP)
		{
			ngl::nlua_table::table_start_pop(L, m_name);
			m_pos = (int32_t)m_vec.size() - 1;
		}
		else
		{
			ngl::nlua_table::table_start_push(L, m_name);
			m_pos = 0;
		}
	}

	~help_nlua()
	{
		if constexpr (POP)
		{
			ngl::nlua_table::table_finish_pop(L, m_name);
		}
		else
		{
			ngl::nlua_table::table_finish_push(L, m_name);
		}
	}

	void push()
	{
	}

	template <typename T>
	void push(const T& adata)
	{
		ngl::nlua_table::table_push(L, m_vec[m_pos++], adata);
	}

	template <typename T, typename ...TARGS>
	void push(const T& adata, const TARGS&... args)
	{
		push(adata);
		push(args...);
	}

	bool pop()
	{
		return true;
	}

	template <typename T>
	bool pop(T& adata)
	{
		if(ngl::nlua_table::table_isnil(L))
		{
			return true;
		}
		return ngl::nlua_table::table_pop(L, m_vec[m_pos--], adata);
	}

	template <typename T, typename ...TARGS>
	bool pop(T& adata, TARGS&... args)
	{
		if (ngl::nlua_table::table_isnil(L))
		{
			return true;
		}
		return pop(args...) && pop(adata);
	}
};

#include "nscript_pbexample.h"
#include "nscript_pbnet.h"
#include "nscript_pbdb.h"
