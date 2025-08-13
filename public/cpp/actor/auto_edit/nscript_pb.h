#pragma once

#include "ndefine.h"
#include "net.pb.h"
#include "db.pb.h"

namespace ngl
{
	template <>
	struct serialize_lua<pbdb::db_account>
	{
		static void stack_push(lua_State* L, const pbdb::db_account& adata)
		{
			lua_newtable(L);
			nlua_stack::stack_push(L, adata.mid(), adata.mroleid(), adata.maccount(), adata.mpassworld(), adata.marea());
		}

		static bool stack_pop(lua_State* L, pbdb::db_account& adata, bool apop = true)
		{
			int64_t	lmid = 0;
			int64_t	lmroleid = 0;
			std::string lmaccount;
			std::string lmpassworld;
			int32_t	lmarea = 0;
			return nlua_stack::stack_pop(L, lmid, lmroleid, lmaccount, lmpassworld, lmarea);
		}

		static void table_push(lua_State* L, const char* aname, const pbdb::db_account& adata)
		{
			stack_push(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, pbdb::db_account& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};


	template <>
	struct serialize_lua<pbdb::brief_activityvalues>
	{
		static void stack_push(lua_State* L, const pbdb::db_account& adata)
		{
			lua_newtable(L);
			nlua_stack::stack_push(L, adata.mid(), adata.mroleid(), adata.maccount(), adata.mpassworld(), adata.marea());
		}

		static bool stack_pop(lua_State* L, pbdb::db_account& adata, bool apop = true)
		{
			int64_t	lmid = 0;
			int64_t	lmroleid = 0;
			std::string lmaccount;
			std::string lmpassworld;
			int32_t	lmarea = 0;
			return nlua_stack::stack_pop(L, lmid, lmroleid, lmaccount, lmpassworld, lmarea);
		}

		static void table_push(lua_State* L, const char* aname, const pbdb::db_account& adata)
		{
			stack_push(L, adata);
			lua_setfield(L, -2, aname);
		}

		static bool table_pop(lua_State* L, const char* aname, pbdb::db_account& adata)
		{
			lua_getfield(L, -1, aname);
			return stack_pop(L, adata);
		}
	};
	

}//namespace ngl