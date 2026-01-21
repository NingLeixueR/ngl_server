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
 // 创建时间【2026-01-20 15:26:56】

#pragma once
#include "ndefine.h"
#include "example.pb.h"
#include "net.pb.h"
#include "db.pb.h"

#include <array>

namespace ngl
{
   template <>
   struct serialize_lua<pbdb::ATTRIBUTE>
   {
       static void stack_push(lua_State* L, const pbdb::ATTRIBUTE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtype","mvalue"} , adata.mtype(),adata.mvalue()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::ATTRIBUTE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmtype;
           int64_t lmvalue;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtype", "mvalue"}, lmtype, lmvalue))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mvalue(lmvalue);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::ATTRIBUTE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::ATTRIBUTE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::POSITION>
   {
       static void stack_push(lua_State* L, const pbdb::POSITION& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmapid","mposition","mangle","mspeed"} , adata.mmapid(),adata.mposition(),adata.mangle(),adata.mspeed()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::POSITION& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmmapid;
           int32_t lmangle;
           int32_t lmspeed;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmapid", "mposition", "mangle", "mspeed"}, lmmapid,  *adata.mutable_mposition(), lmangle, lmspeed))
           {
               return false;
           }
           adata.set_mmapid(lmmapid);
           adata.set_mangle(lmangle);
           adata.set_mspeed(lmspeed);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::POSITION& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::POSITION& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::UNIT>
   {
       static void stack_push(lua_State* L, const pbdb::UNIT& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtype","mid","mposition","mmodules"} , adata.mtype(),adata.mid(),adata.mposition(),adata.mmodules()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::UNIT& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbdb::EUNIT lmtype;
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtype", "mid", "mposition", "mmodules"}, lmtype, lmid,  *adata.mutable_mposition(),  *adata.mutable_mmodules()))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mid(lmid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::UNIT& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::UNIT& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::UNIT_MODULE>
   {
       static void stack_push(lua_State* L, const pbdb::UNIT_MODULE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmtype","mmodules"} , adata.mmtype(),adata.mmodules()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::UNIT_MODULE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmmtype;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmtype", "mmodules"}, lmmtype,  *adata.mutable_mmodules()))
           {
               return false;
           }
           adata.set_mmtype(lmmtype);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::UNIT_MODULE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::UNIT_MODULE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::VECTOR2>
   {
       static void stack_push(lua_State* L, const pbdb::VECTOR2& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mx","my"} , adata.mx(),adata.my()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::VECTOR2& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmx;
           int32_t lmy;
           if(!ngl::nlua_table::table_pop(L, aname, { "mx", "my"}, lmx, lmy))
           {
               return false;
           }
           adata.set_mx(lmx);
           adata.set_my(lmy);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::VECTOR2& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::VECTOR2& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::VECTOR3>
   {
       static void stack_push(lua_State* L, const pbdb::VECTOR3& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mx","my","mz"} , adata.mx(),adata.my(),adata.mz()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::VECTOR3& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmx;
           int32_t lmy;
           int32_t lmz;
           if(!ngl::nlua_table::table_pop(L, aname, { "mx", "my", "mz"}, lmx, lmy, lmz))
           {
               return false;
           }
           adata.set_mx(lmx);
           adata.set_my(lmy);
           adata.set_mz(lmz);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::VECTOR3& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::VECTOR3& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::activity_drawcompliance>
   {
       static void stack_push(lua_State* L, const pbdb::activity_drawcompliance& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mcount","mreward"} , adata.mcount(),adata.mreward()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::activity_drawcompliance& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, aname, { "mcount", "mreward"}, lmcount,  *adata.mutable_mreward()))
           {
               return false;
           }
           adata.set_mcount(lmcount);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::activity_drawcompliance& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::activity_drawcompliance& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::activity_task>
   {
       static void stack_push(lua_State* L, const pbdb::activity_task& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mopen","mclose"} , adata.mopen(),adata.mclose()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::activity_task& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mopen", "mclose"},  *adata.mutable_mopen(),  *adata.mutable_mclose()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::activity_task& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::activity_task& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::brief_activityvalues>
   {
       static void stack_push(lua_State* L, const pbdb::brief_activityvalues& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mactivity_rolelv","mactivity_rolegold"} , adata.mactivity_rolelv(),adata.mactivity_rolegold()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::brief_activityvalues& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mactivity_rolelv", "mactivity_rolegold"},  *adata.mutable_mactivity_rolelv(),  *adata.mutable_mactivity_rolegold()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::brief_activityvalues& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::brief_activityvalues& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::brief_base>
   {
       static void stack_push(lua_State* L, const pbdb::brief_base& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mname","mlv","mmoneygold","mmoneysilver","mvip","mnotalkutc","mcreateutc"} , adata.mname(),adata.mlv(),adata.mmoneygold(),adata.mmoneysilver(),adata.mvip(),adata.mnotalkutc(),adata.mcreateutc()
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
           if(!ngl::nlua_table::table_pop(L, aname, { "mname", "mlv", "mmoneygold", "mmoneysilver", "mvip", "mnotalkutc", "mcreateutc"}, lmname, lmlv, lmmoneygold, lmmoneysilver, lmvip, lmnotalkutc, lmcreateutc))
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
   struct serialize_lua<pbdb::db_account>
   {
       static void stack_push(lua_State* L, const pbdb::db_account& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mroleid","maccount","mpassworld","marea"} , adata.mid(),adata.mroleid(),adata.maccount(),adata.mpassworld(),adata.marea()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_account& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           int64_t lmroleid;
           std::string lmaccount;
           std::string lmpassworld;
           int32_t lmarea;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mroleid", "maccount", "mpassworld", "marea"}, lmid, lmroleid, lmaccount, lmpassworld, lmarea))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mroleid(lmroleid);
           adata.set_maccount(lmaccount);
           adata.set_mpassworld(lmpassworld);
           adata.set_marea(lmarea);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_account& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_account& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_activity>
   {
       static void stack_push(lua_State* L, const pbdb::db_activity& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mtask","mdrawcompliance"} , adata.mid(),adata.mtask(),adata.mdrawcompliance()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_activity& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mtask", "mdrawcompliance"}, lmid,  *adata.mutable_mtask(),  *adata.mutable_mdrawcompliance()))
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
       static void table_push(lua_State * L, const char* aname, const pbdb::db_activity& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_activity& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_activitytimes>
   {
       static void stack_push(lua_State* L, const pbdb::db_activitytimes& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mbeg","mduration","mstart"} , adata.mid(),adata.mbeg(),adata.mduration(),adata.mstart()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_activitytimes& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           int32_t lmbeg;
           int32_t lmduration;
           bool lmstart;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mbeg", "mduration", "mstart"}, lmid, lmbeg, lmduration, lmstart))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mbeg(lmbeg);
           adata.set_mduration(lmduration);
           adata.set_mstart(lmstart);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_activitytimes& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_activitytimes& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_bag>
   {
       static void stack_push(lua_State* L, const pbdb::db_bag& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mitems","mmaxid"} , adata.mid(),adata.mitems(),adata.mmaxid()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_bag& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           int32_t lmmaxid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mitems", "mmaxid"}, lmid,  *adata.mutable_mitems(), lmmaxid))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mmaxid(lmmaxid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_bag& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_bag& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_brief>
   {
       static void stack_push(lua_State* L, const pbdb::db_brief& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mbase","mactivityvalues"} , adata.mid(),adata.mbase(),adata.mactivityvalues()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_brief& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mbase", "mactivityvalues"}, lmid,  *adata.mutable_mbase(),  *adata.mutable_mactivityvalues()))
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
   template <>
   struct serialize_lua<pbdb::db_family>
   {
       static void stack_push(lua_State* L, const pbdb::db_family& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mcreateutc","mname","mleader","mlv","mexp","mmember","mapplylist"} , adata.mid(),adata.mcreateutc(),adata.mname(),adata.mleader(),adata.mlv(),adata.mexp(),adata.mmember(),adata.mapplylist()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_family& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           int32_t lmcreateutc;
           std::string lmname;
           int64_t lmleader;
           int32_t lmlv;
           int32_t lmexp;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mcreateutc", "mname", "mleader", "mlv", "mexp", "mmember", "mapplylist"}, lmid, lmcreateutc, lmname, lmleader, lmlv, lmexp,  *adata.mutable_mmember(),  *adata.mutable_mapplylist()))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mcreateutc(lmcreateutc);
           adata.set_mname(lmname);
           adata.set_mleader(lmleader);
           adata.set_mlv(lmlv);
           adata.set_mexp(lmexp);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_family& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_family& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_familyer>
   {
       static void stack_push(lua_State* L, const pbdb::db_familyer& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mjoinutc","mlastsignutc","mlastleaveutc","mposition"} , adata.mid(),adata.mjoinutc(),adata.mlastsignutc(),adata.mlastleaveutc(),adata.mposition()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_familyer& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           int32_t lmjoinutc;
           int32_t lmlastsignutc;
           int32_t lmlastleaveutc;
           pbdb::db_familyer::eposition lmposition;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mjoinutc", "mlastsignutc", "mlastleaveutc", "mposition"}, lmid, lmjoinutc, lmlastsignutc, lmlastleaveutc, lmposition))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mjoinutc(lmjoinutc);
           adata.set_mlastsignutc(lmlastsignutc);
           adata.set_mlastleaveutc(lmlastleaveutc);
           adata.set_mposition(lmposition);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_familyer& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_familyer& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_friends>
   {
       static void stack_push(lua_State* L, const pbdb::db_friends& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mfriends","mapplyfriends"} , adata.mid(),adata.mfriends(),adata.mapplyfriends()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_friends& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mfriends", "mapplyfriends"}, lmid,  *adata.mutable_mfriends(),  *adata.mutable_mapplyfriends()))
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
       static void table_push(lua_State * L, const char* aname, const pbdb::db_friends& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_friends& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_keyvalue>
   {
       static void stack_push(lua_State* L, const pbdb::db_keyvalue& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mvalue"} , adata.mid(),adata.mvalue()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_keyvalue& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           std::string lmvalue;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mvalue"}, lmid, lmvalue))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mvalue(lmvalue);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_keyvalue& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_keyvalue& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_mail>
   {
       static void stack_push(lua_State* L, const pbdb::db_mail& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mmail"} , adata.mid(),adata.mmail()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_mail& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mmail"}, lmid,  *adata.mutable_mmail()))
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
       static void table_push(lua_State * L, const char* aname, const pbdb::db_mail& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_mail& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_notice>
   {
       static void stack_push(lua_State* L, const pbdb::db_notice& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mnotice","mstarttime","mfinishtime"} , adata.mid(),adata.mnotice(),adata.mstarttime(),adata.mfinishtime()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_notice& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           std::string lmnotice;
           int32_t lmstarttime;
           int32_t lmfinishtime;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mnotice", "mstarttime", "mfinishtime"}, lmid, lmnotice, lmstarttime, lmfinishtime))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mnotice(lmnotice);
           adata.set_mstarttime(lmstarttime);
           adata.set_mfinishtime(lmfinishtime);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_notice& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_notice& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_ranklist>
   {
       static void stack_push(lua_State* L, const pbdb::db_ranklist& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mitems"} , adata.mid(),adata.mitems()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_ranklist& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mitems"}, lmid,  *adata.mutable_mitems()))
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
       static void table_push(lua_State * L, const char* aname, const pbdb::db_ranklist& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_ranklist& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_role>
   {
       static void stack_push(lua_State* L, const pbdb::db_role& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mrecharge"} , adata.mid(),adata.mrecharge()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_role& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mrecharge"}, lmid,  *adata.mutable_mrecharge()))
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
       static void table_push(lua_State * L, const char* aname, const pbdb::db_role& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_role& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_role::recharge>
   {
       static void stack_push(lua_State* L, const pbdb::db_role::recharge& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mrechargeid","mutc"} , adata.mrechargeid(),adata.mutc()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_role::recharge& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmrechargeid;
           int32_t lmutc;
           if(!ngl::nlua_table::table_pop(L, aname, { "mrechargeid", "mutc"}, lmrechargeid, lmutc))
           {
               return false;
           }
           adata.set_mrechargeid(lmrechargeid);
           adata.set_mutc(lmutc);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_role::recharge& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_role::recharge& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_rolekeyvalue>
   {
       static void stack_push(lua_State* L, const pbdb::db_rolekeyvalue& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mdata"} , adata.mid(),adata.mdata()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_rolekeyvalue& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mdata"}, lmid,  *adata.mutable_mdata()))
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
       static void table_push(lua_State * L, const char* aname, const pbdb::db_rolekeyvalue& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_rolekeyvalue& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task>
   {
       static void stack_push(lua_State* L, const pbdb::db_task& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mrundatas","mcompleteddatas"} , adata.mid(),adata.mrundatas(),adata.mcompleteddatas()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_task& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mrundatas", "mcompleteddatas"}, lmid,  *adata.mutable_mrundatas(),  *adata.mutable_mcompleteddatas()))
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
       static void table_push(lua_State * L, const char* aname, const pbdb::db_task& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::complete>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::complete& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mhistory"} , adata.mhistory()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::complete& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mhistory"},  *adata.mutable_mhistory()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_task::complete& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::complete& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::data>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::data& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtaskid","mreceiveutc","mfinshutc","mschedules","mreceive"} , adata.mtaskid(),adata.mreceiveutc(),adata.mfinshutc(),adata.mschedules(),adata.mreceive()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::data& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmtaskid;
           int32_t lmreceiveutc;
           int32_t lmfinshutc;
           bool lmreceive;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtaskid", "mreceiveutc", "mfinshutc", "mschedules", "mreceive"}, lmtaskid, lmreceiveutc, lmfinshutc,  *adata.mutable_mschedules(), lmreceive))
           {
               return false;
           }
           adata.set_mtaskid(lmtaskid);
           adata.set_mreceiveutc(lmreceiveutc);
           adata.set_mfinshutc(lmfinshutc);
           adata.set_mreceive(lmreceive);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_task::data& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::data& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::data_schedule>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::data_schedule& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtype","mvalue","msumint"} , adata.mtype(),adata.mvalue(),adata.msumint()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::data_schedule& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmtype;
           int32_t lmvalue;
           int32_t lmsumint;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtype", "mvalue", "msumint"}, lmtype, lmvalue, lmsumint))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mvalue(lmvalue);
           adata.set_msumint(lmsumint);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_task::data_schedule& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::data_schedule& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_testlua>
   {
       static void stack_push(lua_State* L, const pbdb::db_testlua& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mvalue","mdatas"} , adata.mid(),adata.mvalue(),adata.mdatas()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_testlua& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mvalue", "mdatas"}, lmid,  *adata.mutable_mvalue(),  *adata.mutable_mdatas()))
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
       static void table_push(lua_State * L, const char* aname, const pbdb::db_testlua& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_testlua& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_testlua::luadata>
   {
       static void stack_push(lua_State* L, const pbdb::db_testlua::luadata& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mkey","mval"} , adata.mkey(),adata.mval()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::db_testlua::luadata& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           std::string lmkey;
           std::string lmval;
           if(!ngl::nlua_table::table_pop(L, aname, { "mkey", "mval"}, lmkey, lmval))
           {
               return false;
           }
           adata.set_mkey(lmkey);
           adata.set_mval(lmval);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_testlua::luadata& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_testlua::luadata& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::dbcross_test>
   {
       static void stack_push(lua_State* L, const pbdb::dbcross_test& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mname","mvalue"} , adata.mname(),adata.mvalue()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::dbcross_test& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           std::string lmname;
           if(!ngl::nlua_table::table_pop(L, aname, { "mname", "mvalue"}, lmname,  *adata.mutable_mvalue()))
           {
               return false;
           }
           adata.set_mname(lmname);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::dbcross_test& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::dbcross_test& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::item>
   {
       static void stack_push(lua_State* L, const pbdb::item& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mtid","mcount","mlv","mstar"} , adata.mid(),adata.mtid(),adata.mcount(),adata.mlv(),adata.mstar()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::item& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmid;
           int32_t lmtid;
           int32_t lmcount;
           int32_t lmlv;
           int32_t lmstar;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mtid", "mcount", "mlv", "mstar"}, lmid, lmtid, lmcount, lmlv, lmstar))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mtid(lmtid);
           adata.set_mcount(lmcount);
           adata.set_mlv(lmlv);
           adata.set_mstar(lmstar);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::item& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::item& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::mail>
   {
       static void stack_push(lua_State* L, const pbdb::mail& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mcreateutc","mtid","mprams","mcontent","mitems","mread","mdraw"} , adata.mid(),adata.mcreateutc(),adata.mtid(),adata.mprams(),adata.mcontent(),adata.mitems(),adata.mread(),adata.mdraw()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::mail& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmid;
           int32_t lmcreateutc;
           int32_t lmtid;
           std::string lmprams;
           std::string lmcontent;
           bool lmread;
           bool lmdraw;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mcreateutc", "mtid", "mprams", "mcontent", "mitems", "mread", "mdraw"}, lmid, lmcreateutc, lmtid, lmprams, lmcontent,  *adata.mutable_mitems(), lmread, lmdraw))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mcreateutc(lmcreateutc);
           adata.set_mtid(lmtid);
           adata.set_mprams(lmprams);
           adata.set_mcontent(lmcontent);
           adata.set_mread(lmread);
           adata.set_mdraw(lmdraw);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::mail& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::mail& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::mailitem>
   {
       static void stack_push(lua_State* L, const pbdb::mailitem& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mitemtid","mcount"} , adata.mitemtid(),adata.mcount()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::mailitem& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmitemtid;
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, aname, { "mitemtid", "mcount"}, lmitemtid, lmcount))
           {
               return false;
           }
           adata.set_mitemtid(lmitemtid);
           adata.set_mcount(lmcount);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::mailitem& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::mailitem& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbdb::rankitem>
   {
       static void stack_push(lua_State* L, const pbdb::rankitem& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mvalue","mtime"} , adata.mvalue(),adata.mtime()
               );
       }
       static bool stack_pop(lua_State* L, pbdb::rankitem& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmvalue;
           int32_t lmtime;
           if(!ngl::nlua_table::table_pop(L, aname, { "mvalue", "mtime"}, lmvalue, lmtime))
           {
               return false;
           }
           adata.set_mvalue(lmvalue);
           adata.set_mtime(lmtime);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::rankitem& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::rankitem& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
}//namespace ngl
