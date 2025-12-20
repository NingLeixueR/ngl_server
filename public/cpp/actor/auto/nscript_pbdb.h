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
 // 创建时间【2025-12-20 11:03:02】

#pragma once
#include "ndefine.h"
#include "example.pb.h"
#include "net.pb.h"
#include "db.pb.h"

namespace ngl
{
   template <>
   struct serialize_lua<pbdb::activity_drawcompliance>
   {
       static void stack_push(lua_State* L, const pbdb::activity_drawcompliance& adata)
       {
            ngl::nlua_table::table_push(L, "mcount", adata.mcount(),"mreward", adata.mreward());
       }
       static bool stack_pop(lua_State* L, pbdb::activity_drawcompliance& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, "mcount", lmcount, "mreward",  *adata.mutable_mreward()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::activity_drawcompliance& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::activity_task>
   {
       static void stack_push(lua_State* L, const pbdb::activity_task& adata)
       {
            ngl::nlua_table::table_push(L, "mopen", adata.mopen(),"mclose", adata.mclose());
       }
       static bool stack_pop(lua_State* L, pbdb::activity_task& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mopen",  *adata.mutable_mopen(), "mclose",  *adata.mutable_mclose()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::activity_task& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::brief_activityvalues>
   {
       static void stack_push(lua_State* L, const pbdb::brief_activityvalues& adata)
       {
            ngl::nlua_table::table_push(L, "mactivity_rolelv", adata.mactivity_rolelv(),"mactivity_rolegold", adata.mactivity_rolegold());
       }
       static bool stack_pop(lua_State* L, pbdb::brief_activityvalues& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mactivity_rolelv",  *adata.mutable_mactivity_rolelv(), "mactivity_rolegold",  *adata.mutable_mactivity_rolegold()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::brief_activityvalues& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::brief_base>
   {
       static void stack_push(lua_State* L, const pbdb::brief_base& adata)
       {
            ngl::nlua_table::table_push(L, "mname", adata.mname(),"mlv", adata.mlv(),"mmoneygold", adata.mmoneygold(),"mmoneysilver", adata.mmoneysilver(),"mvip", adata.mvip(),"mnotalkutc", adata.mnotalkutc(),"mcreateutc", adata.mcreateutc());
       }
       static bool stack_pop(lua_State* L, pbdb::brief_base& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
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
           if(!ngl::nlua_table::table_pop(L, "mname", lmname, "mlv", lmlv, "mmoneygold", lmmoneygold, "mmoneysilver", lmmoneysilver, "mvip", lmvip, "mnotalkutc", lmnotalkutc, "mcreateutc", lmcreateutc))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::brief_base& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_account>
   {
       static void stack_push(lua_State* L, const pbdb::db_account& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mroleid", adata.mroleid(),"maccount", adata.maccount(),"mpassworld", adata.mpassworld(),"marea", adata.marea());
       }
       static bool stack_pop(lua_State* L, pbdb::db_account& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           int64_t lmroleid;
           std::string lmaccount;
           std::string lmpassworld;
           int32_t lmarea;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mroleid", lmroleid, "maccount", lmaccount, "mpassworld", lmpassworld, "marea", lmarea))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_account& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_activity>
   {
       static void stack_push(lua_State* L, const pbdb::db_activity& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mtask", adata.mtask(),"mdrawcompliance", adata.mdrawcompliance());
       }
       static bool stack_pop(lua_State* L, pbdb::db_activity& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mtask",  *adata.mutable_mtask(), "mdrawcompliance",  *adata.mutable_mdrawcompliance()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_activity& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_activitytimes>
   {
       static void stack_push(lua_State* L, const pbdb::db_activitytimes& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mbeg", adata.mbeg(),"mduration", adata.mduration(),"mstart", adata.mstart());
       }
       static bool stack_pop(lua_State* L, pbdb::db_activitytimes& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           int32_t lmbeg;
           int32_t lmduration;
           bool lmstart;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mbeg", lmbeg, "mduration", lmduration, "mstart", lmstart))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_activitytimes& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_bag>
   {
       static void stack_push(lua_State* L, const pbdb::db_bag& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mitems", adata.mitems(),"mmaxid", adata.mmaxid());
       }
       static bool stack_pop(lua_State* L, pbdb::db_bag& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           int32_t lmmaxid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mitems",  *adata.mutable_mitems(), "mmaxid", lmmaxid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_bag& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_brief>
   {
       static void stack_push(lua_State* L, const pbdb::db_brief& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mbase", adata.mbase(),"mactivityvalues", adata.mactivityvalues());
       }
       static bool stack_pop(lua_State* L, pbdb::db_brief& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mbase",  *adata.mutable_mbase(), "mactivityvalues",  *adata.mutable_mactivityvalues()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_brief& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_family>
   {
       static void stack_push(lua_State* L, const pbdb::db_family& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mcreateutc", adata.mcreateutc(),"mname", adata.mname(),"mleader", adata.mleader(),"mlv", adata.mlv(),"mexp", adata.mexp(),"mmember", adata.mmember(),"mapplylist", adata.mapplylist());
       }
       static bool stack_pop(lua_State* L, pbdb::db_family& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           int32_t lmcreateutc;
           std::string lmname;
           int64_t lmleader;
           int32_t lmlv;
           int32_t lmexp;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mcreateutc", lmcreateutc, "mname", lmname, "mleader", lmleader, "mlv", lmlv, "mexp", lmexp, "mmember",  *adata.mutable_mmember(), "mapplylist",  *adata.mutable_mapplylist()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_family& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_familyer>
   {
       static void stack_push(lua_State* L, const pbdb::db_familyer& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mjoinutc", adata.mjoinutc(),"mlastsignutc", adata.mlastsignutc(),"mlastleaveutc", adata.mlastleaveutc(),"mposition", adata.mposition());
       }
       static bool stack_pop(lua_State* L, pbdb::db_familyer& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           int32_t lmjoinutc;
           int32_t lmlastsignutc;
           int32_t lmlastleaveutc;
           pbdb::db_familyer::eposition lmposition;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mjoinutc", lmjoinutc, "mlastsignutc", lmlastsignutc, "mlastleaveutc", lmlastleaveutc, "mposition", lmposition))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_familyer& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_friends>
   {
       static void stack_push(lua_State* L, const pbdb::db_friends& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mfriends", adata.mfriends(),"mapplyfriends", adata.mapplyfriends());
       }
       static bool stack_pop(lua_State* L, pbdb::db_friends& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mfriends",  *adata.mutable_mfriends(), "mapplyfriends",  *adata.mutable_mapplyfriends()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_friends& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_keyvalue>
   {
       static void stack_push(lua_State* L, const pbdb::db_keyvalue& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mvalue", adata.mvalue());
       }
       static bool stack_pop(lua_State* L, pbdb::db_keyvalue& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           std::string lmvalue;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mvalue", lmvalue))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_keyvalue& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_mail>
   {
       static void stack_push(lua_State* L, const pbdb::db_mail& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mmail", adata.mmail());
       }
       static bool stack_pop(lua_State* L, pbdb::db_mail& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mmail",  *adata.mutable_mmail()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_mail& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_notice>
   {
       static void stack_push(lua_State* L, const pbdb::db_notice& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mnotice", adata.mnotice(),"mstarttime", adata.mstarttime(),"mfinishtime", adata.mfinishtime());
       }
       static bool stack_pop(lua_State* L, pbdb::db_notice& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           std::string lmnotice;
           int32_t lmstarttime;
           int32_t lmfinishtime;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mnotice", lmnotice, "mstarttime", lmstarttime, "mfinishtime", lmfinishtime))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_notice& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_ranklist>
   {
       static void stack_push(lua_State* L, const pbdb::db_ranklist& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mitems", adata.mitems());
       }
       static bool stack_pop(lua_State* L, pbdb::db_ranklist& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mitems",  *adata.mutable_mitems()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_ranklist& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_role>
   {
       static void stack_push(lua_State* L, const pbdb::db_role& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mrecharge", adata.mrecharge());
       }
       static bool stack_pop(lua_State* L, pbdb::db_role& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mrecharge",  *adata.mutable_mrecharge()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_role& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_role::recharge>
   {
       static void stack_push(lua_State* L, const pbdb::db_role::recharge& adata)
       {
            ngl::nlua_table::table_push(L, "mrechargeid", adata.mrechargeid(),"mutc", adata.mutc());
       }
       static bool stack_pop(lua_State* L, pbdb::db_role::recharge& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmrechargeid;
           int32_t lmutc;
           if(!ngl::nlua_table::table_pop(L, "mrechargeid", lmrechargeid, "mutc", lmutc))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_role::recharge& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_rolekeyvalue>
   {
       static void stack_push(lua_State* L, const pbdb::db_rolekeyvalue& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mdata", adata.mdata());
       }
       static bool stack_pop(lua_State* L, pbdb::db_rolekeyvalue& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mdata",  *adata.mutable_mdata()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_rolekeyvalue& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task>
   {
       static void stack_push(lua_State* L, const pbdb::db_task& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mrundatas", adata.mrundatas(),"mcompleteddatas", adata.mcompleteddatas());
       }
       static bool stack_pop(lua_State* L, pbdb::db_task& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mrundatas",  *adata.mutable_mrundatas(), "mcompleteddatas",  *adata.mutable_mcompleteddatas()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::complete>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::complete& adata)
       {
            ngl::nlua_table::table_push(L, "mhistory", adata.mhistory());
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::complete& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mhistory",  *adata.mutable_mhistory()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::complete& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::data>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::data& adata)
       {
            ngl::nlua_table::table_push(L, "mtaskid", adata.mtaskid(),"mreceiveutc", adata.mreceiveutc(),"mfinshutc", adata.mfinshutc(),"mschedules", adata.mschedules(),"mreceive", adata.mreceive());
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::data& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmtaskid;
           int32_t lmreceiveutc;
           int32_t lmfinshutc;
           bool lmreceive;
           if(!ngl::nlua_table::table_pop(L, "mtaskid", lmtaskid, "mreceiveutc", lmreceiveutc, "mfinshutc", lmfinshutc, "mschedules",  *adata.mutable_mschedules(), "mreceive", lmreceive))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::data& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::data_schedule>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::data_schedule& adata)
       {
            ngl::nlua_table::table_push(L, "mtype", adata.mtype(),"mvalue", adata.mvalue(),"msumint", adata.msumint());
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::data_schedule& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmtype;
           int32_t lmvalue;
           int32_t lmsumint;
           if(!ngl::nlua_table::table_pop(L, "mtype", lmtype, "mvalue", lmvalue, "msumint", lmsumint))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::data_schedule& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_testlua>
   {
       static void stack_push(lua_State* L, const pbdb::db_testlua& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mvalue", adata.mvalue(),"mdatas", adata.mdatas());
       }
       static bool stack_pop(lua_State* L, pbdb::db_testlua& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mvalue",  *adata.mutable_mvalue(), "mdatas",  *adata.mutable_mdatas()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_testlua& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::db_testlua::luadata>
   {
       static void stack_push(lua_State* L, const pbdb::db_testlua::luadata& adata)
       {
            ngl::nlua_table::table_push(L, "mkey", adata.mkey(),"mval", adata.mval());
       }
       static bool stack_pop(lua_State* L, pbdb::db_testlua::luadata& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           std::string lmkey;
           std::string lmval;
           if(!ngl::nlua_table::table_pop(L, "mkey", lmkey, "mval", lmval))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_testlua::luadata& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::dbcross_test>
   {
       static void stack_push(lua_State* L, const pbdb::dbcross_test& adata)
       {
            ngl::nlua_table::table_push(L, "mname", adata.mname(),"mvalue", adata.mvalue());
       }
       static bool stack_pop(lua_State* L, pbdb::dbcross_test& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           std::string lmname;
           if(!ngl::nlua_table::table_pop(L, "mname", lmname, "mvalue",  *adata.mutable_mvalue()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::dbcross_test& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::item>
   {
       static void stack_push(lua_State* L, const pbdb::item& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mtid", adata.mtid(),"mcount", adata.mcount(),"mlv", adata.mlv(),"mstar", adata.mstar());
       }
       static bool stack_pop(lua_State* L, pbdb::item& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmid;
           int32_t lmtid;
           int32_t lmcount;
           int32_t lmlv;
           int32_t lmstar;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mtid", lmtid, "mcount", lmcount, "mlv", lmlv, "mstar", lmstar))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::item& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::mail>
   {
       static void stack_push(lua_State* L, const pbdb::mail& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mcreateutc", adata.mcreateutc(),"mtid", adata.mtid(),"mprams", adata.mprams(),"mcontent", adata.mcontent(),"mitems", adata.mitems(),"mread", adata.mread(),"mdraw", adata.mdraw());
       }
       static bool stack_pop(lua_State* L, pbdb::mail& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
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
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mcreateutc", lmcreateutc, "mtid", lmtid, "mprams", lmprams, "mcontent", lmcontent, "mitems",  *adata.mutable_mitems(), "mread", lmread, "mdraw", lmdraw))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::mail& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::mailitem>
   {
       static void stack_push(lua_State* L, const pbdb::mailitem& adata)
       {
            ngl::nlua_table::table_push(L, "mitemtid", adata.mitemtid(),"mcount", adata.mcount());
       }
       static bool stack_pop(lua_State* L, pbdb::mailitem& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmitemtid;
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, "mitemtid", lmitemtid, "mcount", lmcount))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::mailitem& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
   template <>
   struct serialize_lua<pbdb::rankitem>
   {
       static void stack_push(lua_State* L, const pbdb::rankitem& adata)
       {
            ngl::nlua_table::table_push(L, "mvalue", adata.mvalue(),"mtime", adata.mtime());
       }
       static bool stack_pop(lua_State* L, pbdb::rankitem& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmvalue;
           int32_t lmtime;
           if(!ngl::nlua_table::table_pop(L, "mvalue", lmvalue, "mtime", lmtime))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::rankitem& adata)
       {
           ngl::nlua_table::table_start_pop(L, aname);
           if (!stack_pop(L, adata, false))
           {
               return false;
           }
           ngl::nlua_table::table_finish_pop(L, aname);
           return true;
       }
   };
}//namespace ngl
