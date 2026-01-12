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

 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2026-01-11 18:30:55】

#pragma once
#include "ndefine.h"
#include "example.pb.h"
#include "net.pb.h"
#include "db.pb.h"

namespace ngl
{
   template <>
   struct serialize_lua<pbdb::brief_base>
   {
       static void stack_push(lua_State* L, const pbdb::brief_base& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, 
                { "mname", "mlv", "mmoneygold", "mmoneysilver", "mvip", "mnotalkutc", "mcreateutc" }
            , adata.mname(), adata.mlv(), adata.mmoneygold(), adata.mmoneysilver(), adata.mvip(), adata.mnotalkutc(), adata.mcreateutc()
            );
       }
       static bool stack_pop(lua_State* L, pbdb::brief_base& adata, bool apop = true, const char* aname = nullptr)
       {
           if (lua_isnil(L, -1))
           {
               return true;
           }
           std::string lmname;
           int32_t lmlv;
           int32_t lmmoneygold;
           int32_t lmmoneysilver;
           int32_t lmvip;
           int32_t lmnotalkutc;
           int32_t lmcreateutc;
           if(!ngl::nlua_table::table_pop(L, aname, 
               { "mname", "mlv", "mmoneygold", "mmoneysilver", "mvip", "mnotalkutc", "mcreateutc" },
               lmname, lmlv, lmmoneygold, lmmoneysilver, lmvip, lmnotalkutc, lmcreateutc))
           {
               return false;
           }
           adata.set_mname(lmname);
           adata.set_mlv(lmlv);
           adata.set_mmoneygold(lmmoneygold);
           adata.set_mmoneysilver(lmmoneysilver);
           adata.set_mvip(lmvip);
           adata.set_mnotalkutc(lmnotalkutc);
           adata.set_mcreateutc(lmcreateutc);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::brief_base& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::brief_base& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_brief>
   {
       static void stack_push(lua_State* L, const pbdb::db_brief& adata, const char* aname = nullptr)
       {
           ngl::nlua_table::table_push(L, aname, { "mid","mbase","mactivityvalues" }, adata.mid(), adata.mbase(), adata.mactivityvalues());
       }
       static bool stack_pop(lua_State* L, pbdb::db_brief& adata, bool apop = true, const char* aname = nullptr)
       {
           if (lua_isnil(L, -1))
           {
               return true;
           }
           int64_t lmid;
           if (!ngl::nlua_table::table_pop(L, aname, { "mid","mbase","mactivityvalues" }, lmid, *adata.mutable_mbase(), *adata.mutable_mactivityvalues()))
           {
               return false;
           }
           adata.set_mid(lmid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_brief& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_brief& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };

#define dserialize_lua(STRUCT,NAME) \
   template <>\
   struct serialize_lua<STRUCT::NAME>\
   {\
       static void stack_push(lua_State* L, const STRUCT::NAME& adata, const char* aname = nullptr){}\
       static bool stack_pop(lua_State* L, STRUCT::NAME& adata, bool apop = true, const char* aname = nullptr){return true;}\
       static void table_push(lua_State* L, const char* aname, const STRUCT::NAME& adata){ }\
       static bool table_pop(lua_State* L, const char* aname, STRUCT::NAME& adata){return true;}\
   };

   dserialize_lua(pbdb, brief_activityvalues)
   dserialize_lua(pbdb, db_mail)
   dserialize_lua(pbdb, db_account)
       dserialize_lua(pbdb, db_family)
       dserialize_lua(pbdb, db_familyer)
       dserialize_lua(pbdb, db_friends)
       dserialize_lua(pbdb, db_testlua)
       dserialize_lua(pbdb, db_ranklist)
       dserialize_lua(pbdb, db_keyvalue)
       dserialize_lua(pbdb, db_notice)
       dserialize_lua(pbdb, db_bag)
       dserialize_lua(pbdb, db_task)
       dserialize_lua(pbdb, db_rolekeyvalue)
       dserialize_lua(pbdb, db_role)
       dserialize_lua(pbdb, db_activitytimes)
       dserialize_lua(pbdb, db_activity)
       dserialize_lua(pbnet, PROBUFF_NET_TESTLUA)
      /* dserialize_lua(db_task)
       dserialize_lua(db_task)
       dserialize_lua(db_task)*/
       

}//namespace ngl
