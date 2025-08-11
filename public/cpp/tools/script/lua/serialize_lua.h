#pragma once

#include "lua.hpp"
#include "nguid.h"

#include <string>
#include <map>

//# define LOG_SCRIPT(...)  ::ngl::log_error()->print(__VA_ARGS__)
# define LOG_SCRIPT(...)  std::cout << std::format(__VA_ARGS__) << std::endl

namespace ngl
{
	template <typename T>
	struct serialize_lua
	{
		static void stack_push(lua_State* L, const T& adata)
		{
			adata.nlua_push(L, nullptr);
		}

		static bool stack_pop(lua_State* L, T& adata, bool apop = true)
		{
			if (!adata.nlua_pop(L))
			{
				return false;
			}
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, const T& adata)
		{
			adata.nlua_push(L, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, T& adata)
		{
			return adata.nlua_pop(L, aname);
		}
	};

	template <>
	struct serialize_lua<double>
	{
		static void stack_push(lua_State* L, double adata)
		{
			lua_pushnumber(L, adata);
		}

		static bool stack_pop(lua_State* L, double& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tonumber(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, double adata)
		{
			lua_pushnumber(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, double& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<float>
	{
		static void stack_push(lua_State* L, float adata)
		{
			lua_pushnumber(L, adata);
		}

		static bool stack_pop(lua_State* L, float& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tonumber(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, float adata)
		{
			lua_pushnumber(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, float& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<bool>
	{
		static void stack_push(lua_State* L, bool adata)
		{
			lua_pushboolean(L, adata);
		}

		static bool stack_pop(lua_State* L, bool& adata, bool apop = true)
		{
			if (!lua_isboolean(L, -1))
			{
				return false;
			}
			adata = lua_toboolean(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, bool adata)
		{
			lua_pushboolean(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, bool& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<std::string>
	{
		static void stack_push(lua_State* L, const std::string& adata)
		{
			lua_pushstring(L, adata.c_str());
		}

		static bool stack_pop(lua_State* L, std::string& adata, bool apop = true)
		{
			if (!lua_isstring(L, -1))
			{
				return false;
			}
			adata = lua_tostring(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, const std::string& adata)
		{
			lua_pushstring(L, adata.c_str());
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, std::string& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<const char*>
	{
		static void stack_push(lua_State* L, const char* adata)
		{
			lua_pushstring(L, adata);
		}

		static bool stack_pop(lua_State* L, const char*& adata, bool apop = true)
		{
			if (!lua_isstring(L, -1))
			{
				return false;
			}
			adata = lua_tostring(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, const char* adata)
		{
			lua_pushstring(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, const char*& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<int8_t>
	{
		static void stack_push(lua_State* L, int8_t adata)
		{
			lua_pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, int8_t& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tointeger(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, int8_t adata)
		{
			lua_pushinteger(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, int8_t& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<int16_t>
	{
		static void stack_push(lua_State* L, int16_t adata)
		{
			lua_pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, int16_t& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tointeger(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, int16_t adata)
		{
			lua_pushinteger(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, int16_t& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<int32_t>
	{
		static void stack_push(lua_State* L, int32_t adata)
		{
			lua_pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, int32_t& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tointeger(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, int32_t adata)
		{
			lua_pushinteger(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, int32_t& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<int64_t>
	{
		static void stack_push(lua_State* L, int64_t adata)
		{
			lua_pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, int64_t& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tointeger(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, int64_t adata)
		{
			lua_pushinteger(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, int64_t& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<uint8_t>
	{
		static void stack_push(lua_State* L, uint8_t adata)
		{
			lua_pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, uint8_t& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tointeger(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, uint8_t adata)
		{
			lua_pushinteger(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, uint8_t& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<uint16_t>
	{
		static void stack_push(lua_State* L, uint16_t adata)
		{
			lua_pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, uint16_t& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tointeger(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, uint16_t adata)
		{
			lua_pushinteger(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, uint16_t& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<uint32_t>
	{
		static void stack_push(lua_State* L, uint32_t adata)
		{
			lua_pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, uint32_t& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tointeger(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, uint32_t adata)
		{
			lua_pushinteger(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, uint32_t& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	template <>
	struct serialize_lua<uint64_t>
	{
		static void stack_push(lua_State* L, uint64_t adata)
		{
			lua_pushinteger(L, adata);
		}

		static bool stack_pop(lua_State* L, uint64_t& adata, bool apop = true)
		{
			if (!lua_isnumber(L, -1))
			{
				return false;
			}
			adata = lua_tointeger(L, -1);
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static void table_push(lua_State* L, const char* aname, uint64_t adata)
		{
			lua_pushinteger(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, uint64_t& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	struct tools_pop_tables
	{
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
			std::map<int32_t, T> lmap;
			if (!tools_pop_tables::keyvalues(L, lmap))
			{
				return false;
			}
			for (const auto& item : lmap)
			{
				adata.push_back(item.second);
			}
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, std::vector<T>& adata)
		{
			lua_getfield(L, -1, aname);
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
			std::map<int32_t, T> lmap;
			if (!tools_pop_tables::keyvalues(L, lmap))
			{
				return false;
			}
			for (const auto& item : lmap)
			{
				adata.insert(item.second);
			}
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, std::set<T>& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};

	// 只有各种int 与 string
	template <typename T>
	struct tools_table_key
	{
		static void key(lua_State* L, T adata)
		{
			lua_rawseti(L, -2, adata);
		}
	};

	template <>
	struct tools_table_key<std::string>
	{
		static void key(lua_State* L, const std::string& adata)
		{
			lua_setfield(L, -2, adata.c_str());
		}
	};

	//typename TMAP::key_type;
	//typename TMAP::value_type::second_type;
	template <typename KEY, typename VAL>
	struct serialize_lua<std::map<KEY, VAL>>
	{
		static void stack_push(lua_State* L, const std::map<KEY, VAL>& adata)
		{
			lua_newtable(L);
			for (const auto& item : adata)
			{
				serialize_lua<VAL>::stack_push(L, item.second);
				tools_table_key<KEY>::key(L, item.first);
			}
		}

		static void table_push(lua_State* L, const char* aname, const std::map<KEY, VAL>& adata)
		{
			stack_push(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool stack_pop(lua_State* L, std::map<KEY, VAL>& adata, bool apop = true)
		{
			if (!tools_pop_tables::keyvalues(L, adata))
			{
				return false;
			}
			if (apop)
			{
				lua_pop(L, 1);
			}
			return true;
		}

		static bool table_pop(lua_State* L, const char* aname, std::map<KEY, VAL>& adata)
		{
			lua_getfield(L, -1, aname);
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
			int32_t lpos = -sizeof...(TARGS);
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
			return table_pop(ainfo.L, args...);
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
}//namespace ngl


#define def_parmname(...) \
	static std::vector<const char*>& parms()\
	{\
		static char lbuff[] = #__VA_ARGS__;\
		static std::vector<const char*> lvec = ngl::tools::split_str(lbuff, sizeof(lbuff));\
		return lvec;\
	}


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
			ngl::nlua_table::table_start_pop(aL, aname);
			m_pos = m_vec.size() - 1;
		}
		else
		{
			ngl::nlua_table::table_start_push(aL, aname);
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

	template <typename T>
	bool pop(T& adata)
	{
		return ngl::nlua_table::table_pop(L, m_vec[m_pos--], adata);
	}

	template <typename T, typename ...TARGS>
	bool pop(T& adata, TARGS&... args)
	{
		return pop(args...) && pop(adata);
	}
};


#define def_nlua_push(...)													\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const			\
	{																		\
		help_nlua<false> ltemp(aL, aname, parms());							\
		ltemp.push(__VA_ARGS__);											\
	}

#define def_nlua_pop(...)													\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)				\
	{																		\
		help_nlua<true> ltemp(aL, aname, parms());							\
		return ltemp.pop(__VA_ARGS__);										\
	}

#define def_nlua_function(...)	\
	def_parmname(__VA_ARGS__)\
	def_nlua_push(__VA_ARGS__)\
	def_nlua_pop(__VA_ARGS__)\


namespace ngl
{
	// # 测试

	struct k0
	{
		int m_v1;
		double m_v2;
		std::string m_v3;

		def_nlua_function(m_v1, m_v2, m_v3)
	};

	struct k1
	{
		int m_v1;
		double m_v2;
		std::string m_v3;
		std::vector<int32_t> m_v4;
		std::map<int32_t, std::string> m_v5;
		std::map<std::string, std::string> m_v6;
		std::map<std::string, k0> m_v7;

		def_nlua_function(m_v1, m_v2, m_v3, m_v4, m_v5, m_v6, m_v7)
	};

	struct k2
	{
		int m_v1;
		k1 m_v2;

		def_nlua_function(m_v1, m_v2)
	};
}//namespace ngl