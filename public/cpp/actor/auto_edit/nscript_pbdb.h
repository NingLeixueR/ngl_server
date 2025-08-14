 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2025-08-14 17:39:07】

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
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mcount(),adata.mreward());
       }
       static bool stack_pop(lua_State* L, pbdb::activity_drawcompliance& adata, bool apop = true)
       {
           int32_t lmcount;
           if(!nlua_stack::stack_pop(L, lmcount,  *adata.mutable_mreward()))
           {
               return false;
           }
           adata.set_mcount(lmcount);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::activity_drawcompliance& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::activity_drawcompliance& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::activity_task>
   {
       static void stack_push(lua_State* L, const pbdb::activity_task& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mopen(),adata.mclose());
       }
       static bool stack_pop(lua_State* L, pbdb::activity_task& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mopen(),  *adata.mutable_mclose()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::activity_task& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::activity_task& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::brief_activityvalues>
   {
       static void stack_push(lua_State* L, const pbdb::brief_activityvalues& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mactivity_rolelv(),adata.mactivity_rolegold());
       }
       static bool stack_pop(lua_State* L, pbdb::brief_activityvalues& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mactivity_rolelv(),  *adata.mutable_mactivity_rolegold()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::brief_activityvalues& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::brief_activityvalues& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_account>
   {
       static void stack_push(lua_State* L, const pbdb::db_account& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mroleid(),adata.maccount(),adata.mpassworld(),adata.marea());
       }
       static bool stack_pop(lua_State* L, pbdb::db_account& adata, bool apop = true)
       {
           int64_t lmid;
           int64_t lmroleid;
           std::string lmaccount;
           std::string lmpassworld;
           int32_t lmarea;
           if(!nlua_stack::stack_pop(L, lmid, lmroleid, lmaccount, lmpassworld, lmarea))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mroleid(lmroleid);
           adata.set_maccount(lmaccount);
           adata.set_mpassworld(lmpassworld);
           adata.set_marea(lmarea);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_account& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_account& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_activity>
   {
       static void stack_push(lua_State* L, const pbdb::db_activity& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mtask(),adata.mdrawcompliance());
       }
       static bool stack_pop(lua_State* L, pbdb::db_activity& adata, bool apop = true)
       {
           int64_t lmid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mtask(),  *adata.mutable_mdrawcompliance()))
           {
               return false;
           }
           adata.set_mid(lmid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_activity& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_activity& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_activitytimes>
   {
       static void stack_push(lua_State* L, const pbdb::db_activitytimes& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mbeg(),adata.mduration(),adata.mstart());
       }
       static bool stack_pop(lua_State* L, pbdb::db_activitytimes& adata, bool apop = true)
       {
           int64_t lmid;
           int32_t lmbeg;
           int32_t lmduration;
           bool lmstart;
           if(!nlua_stack::stack_pop(L, lmid, lmbeg, lmduration, lmstart))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mbeg(lmbeg);
           adata.set_mduration(lmduration);
           adata.set_mstart(lmstart);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_activitytimes& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_activitytimes& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_bag>
   {
       static void stack_push(lua_State* L, const pbdb::db_bag& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mitems(),adata.mmaxid());
       }
       static bool stack_pop(lua_State* L, pbdb::db_bag& adata, bool apop = true)
       {
           int64_t lmid;
           int32_t lmmaxid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mitems(), lmmaxid))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mmaxid(lmmaxid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_bag& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_bag& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_brief>
   {
       static void stack_push(lua_State* L, const pbdb::db_brief& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mname(),adata.mlv(),adata.mmoneygold(),adata.mmoneysilver(),adata.mvip(),adata.mnotalkutc(),adata.mcreateutc(),adata.mactivityvalues());
       }
       static bool stack_pop(lua_State* L, pbdb::db_brief& adata, bool apop = true)
       {
           int64_t lmid;
           std::string lmname;
           int32_t lmlv;
           int32_t lmmoneygold;
           int32_t lmmoneysilver;
           int32_t lmvip;
           int32_t lmnotalkutc;
           int32_t lmcreateutc;
           if(!nlua_stack::stack_pop(L, lmid, lmname, lmlv, lmmoneygold, lmmoneysilver, lmvip, lmnotalkutc, lmcreateutc,  *adata.mutable_mactivityvalues()))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mname(lmname);
           adata.set_mlv(lmlv);
           adata.set_mmoneygold(lmmoneygold);
           adata.set_mmoneysilver(lmmoneysilver);
           adata.set_mvip(lmvip);
           adata.set_mnotalkutc(lmnotalkutc);
           adata.set_mcreateutc(lmcreateutc);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_brief& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_brief& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_family>
   {
       static void stack_push(lua_State* L, const pbdb::db_family& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mcreateutc(),adata.mname(),adata.mleader(),adata.mlv(),adata.mexp(),adata.mmember(),adata.mapplylist());
       }
       static bool stack_pop(lua_State* L, pbdb::db_family& adata, bool apop = true)
       {
           int64_t lmid;
           int32_t lmcreateutc;
           std::string lmname;
           int64_t lmleader;
           int32_t lmlv;
           int32_t lmexp;
           if(!nlua_stack::stack_pop(L, lmid, lmcreateutc, lmname, lmleader, lmlv, lmexp,  *adata.mutable_mmember(),  *adata.mutable_mapplylist()))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mcreateutc(lmcreateutc);
           adata.set_mname(lmname);
           adata.set_mleader(lmleader);
           adata.set_mlv(lmlv);
           adata.set_mexp(lmexp);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_family& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_family& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_familyer>
   {
       static void stack_push(lua_State* L, const pbdb::db_familyer& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mjoinutc(),adata.mlastsignutc(),adata.mlastleaveutc(),adata.mposition());
       }
       static bool stack_pop(lua_State* L, pbdb::db_familyer& adata, bool apop = true)
       {
           int64_t lmid;
           int32_t lmjoinutc;
           int32_t lmlastsignutc;
           int32_t lmlastleaveutc;
           pbdb::db_familyer::eposition lmposition;
           if(!nlua_stack::stack_pop(L, lmid, lmjoinutc, lmlastsignutc, lmlastleaveutc, lmposition))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mjoinutc(lmjoinutc);
           adata.set_mlastsignutc(lmlastsignutc);
           adata.set_mlastleaveutc(lmlastleaveutc);
           adata.set_mposition(lmposition);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_familyer& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_familyer& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_friends>
   {
       static void stack_push(lua_State* L, const pbdb::db_friends& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mfriends(),adata.mapplyfriends());
       }
       static bool stack_pop(lua_State* L, pbdb::db_friends& adata, bool apop = true)
       {
           int64_t lmid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mfriends(),  *adata.mutable_mapplyfriends()))
           {
               return false;
           }
           adata.set_mid(lmid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_friends& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_friends& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_keyvalue>
   {
       static void stack_push(lua_State* L, const pbdb::db_keyvalue& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mvalue());
       }
       static bool stack_pop(lua_State* L, pbdb::db_keyvalue& adata, bool apop = true)
       {
           int64_t lmid;
           std::string lmvalue;
           if(!nlua_stack::stack_pop(L, lmid, lmvalue))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mvalue(lmvalue);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_keyvalue& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_keyvalue& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_mail>
   {
       static void stack_push(lua_State* L, const pbdb::db_mail& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mmail());
       }
       static bool stack_pop(lua_State* L, pbdb::db_mail& adata, bool apop = true)
       {
           int64_t lmid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mmail()))
           {
               return false;
           }
           adata.set_mid(lmid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_mail& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_mail& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_notice>
   {
       static void stack_push(lua_State* L, const pbdb::db_notice& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mnotice(),adata.mstarttime(),adata.mfinishtime());
       }
       static bool stack_pop(lua_State* L, pbdb::db_notice& adata, bool apop = true)
       {
           int64_t lmid;
           std::string lmnotice;
           int32_t lmstarttime;
           int32_t lmfinishtime;
           if(!nlua_stack::stack_pop(L, lmid, lmnotice, lmstarttime, lmfinishtime))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mnotice(lmnotice);
           adata.set_mstarttime(lmstarttime);
           adata.set_mfinishtime(lmfinishtime);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_notice& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_notice& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_ranklist>
   {
       static void stack_push(lua_State* L, const pbdb::db_ranklist& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mitems());
       }
       static bool stack_pop(lua_State* L, pbdb::db_ranklist& adata, bool apop = true)
       {
           int64_t lmid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mitems()))
           {
               return false;
           }
           adata.set_mid(lmid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_ranklist& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_ranklist& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_role>
   {
       static void stack_push(lua_State* L, const pbdb::db_role& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mrecharge());
       }
       static bool stack_pop(lua_State* L, pbdb::db_role& adata, bool apop = true)
       {
           int64_t lmid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mrecharge()))
           {
               return false;
           }
           adata.set_mid(lmid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_role& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_role& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_role::recharge>
   {
       static void stack_push(lua_State* L, const pbdb::db_role::recharge& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mrechargeid(),adata.mutc());
       }
       static bool stack_pop(lua_State* L, pbdb::db_role::recharge& adata, bool apop = true)
       {
           int32_t lmrechargeid;
           int32_t lmutc;
           if(!nlua_stack::stack_pop(L, lmrechargeid, lmutc))
           {
               return false;
           }
           adata.set_mrechargeid(lmrechargeid);
           adata.set_mutc(lmutc);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_role::recharge& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_role::recharge& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_rolekeyvalue>
   {
       static void stack_push(lua_State* L, const pbdb::db_rolekeyvalue& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mdata());
       }
       static bool stack_pop(lua_State* L, pbdb::db_rolekeyvalue& adata, bool apop = true)
       {
           int64_t lmid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mdata()))
           {
               return false;
           }
           adata.set_mid(lmid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_rolekeyvalue& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_rolekeyvalue& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task>
   {
       static void stack_push(lua_State* L, const pbdb::db_task& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mrundatas(),adata.mcompleteddatas());
       }
       static bool stack_pop(lua_State* L, pbdb::db_task& adata, bool apop = true)
       {
           int64_t lmid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mrundatas(),  *adata.mutable_mcompleteddatas()))
           {
               return false;
           }
           adata.set_mid(lmid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_task& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::complete>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::complete& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mhistory());
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::complete& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mhistory()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_task::complete& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::complete& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::data>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::data& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtaskid(),adata.mreceiveutc(),adata.mfinshutc(),adata.mschedules(),adata.mreceive());
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::data& adata, bool apop = true)
       {
           int32_t lmtaskid;
           int32_t lmreceiveutc;
           int32_t lmfinshutc;
           bool lmreceive;
           if(!nlua_stack::stack_pop(L, lmtaskid, lmreceiveutc, lmfinshutc,  *adata.mutable_mschedules(), lmreceive))
           {
               return false;
           }
           adata.set_mtaskid(lmtaskid);
           adata.set_mreceiveutc(lmreceiveutc);
           adata.set_mfinshutc(lmfinshutc);
           adata.set_mreceive(lmreceive);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_task::data& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::data& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_task::data_schedule>
   {
       static void stack_push(lua_State* L, const pbdb::db_task::data_schedule& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtype(),adata.mvalue(),adata.msumint());
       }
       static bool stack_pop(lua_State* L, pbdb::db_task::data_schedule& adata, bool apop = true)
       {
           int32_t lmtype;
           int32_t lmvalue;
           int32_t lmsumint;
           if(!nlua_stack::stack_pop(L, lmtype, lmvalue, lmsumint))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mvalue(lmvalue);
           adata.set_msumint(lmsumint);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_task::data_schedule& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_task::data_schedule& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_testlua>
   {
       static void stack_push(lua_State* L, const pbdb::db_testlua& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mvalue(),adata.mdatas());
       }
       static bool stack_pop(lua_State* L, pbdb::db_testlua& adata, bool apop = true)
       {
           int64_t lmid;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mvalue(),  *adata.mutable_mdatas()))
           {
               return false;
           }
           adata.set_mid(lmid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_testlua& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_testlua& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::db_testlua::luadata>
   {
       static void stack_push(lua_State* L, const pbdb::db_testlua::luadata& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mkey(),adata.mval());
       }
       static bool stack_pop(lua_State* L, pbdb::db_testlua::luadata& adata, bool apop = true)
       {
           std::string lmkey;
           std::string lmval;
           if(!nlua_stack::stack_pop(L, lmkey, lmval))
           {
               return false;
           }
           adata.set_mkey(lmkey);
           adata.set_mval(lmval);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::db_testlua::luadata& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::db_testlua::luadata& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::dbcross_test>
   {
       static void stack_push(lua_State* L, const pbdb::dbcross_test& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mname(),adata.mvalue());
       }
       static bool stack_pop(lua_State* L, pbdb::dbcross_test& adata, bool apop = true)
       {
           std::string lmname;
           if(!nlua_stack::stack_pop(L, lmname,  *adata.mutable_mvalue()))
           {
               return false;
           }
           adata.set_mname(lmname);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::dbcross_test& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::dbcross_test& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::item>
   {
       static void stack_push(lua_State* L, const pbdb::item& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mtid(),adata.mcount(),adata.mlv(),adata.mstar());
       }
       static bool stack_pop(lua_State* L, pbdb::item& adata, bool apop = true)
       {
           int32_t lmid;
           int32_t lmtid;
           int32_t lmcount;
           int32_t lmlv;
           int32_t lmstar;
           if(!nlua_stack::stack_pop(L, lmid, lmtid, lmcount, lmlv, lmstar))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mtid(lmtid);
           adata.set_mcount(lmcount);
           adata.set_mlv(lmlv);
           adata.set_mstar(lmstar);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::item& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::item& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::mail>
   {
       static void stack_push(lua_State* L, const pbdb::mail& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mcreateutc(),adata.mtid(),adata.mprams(),adata.mcontent(),adata.mitems(),adata.mread(),adata.mdraw());
       }
       static bool stack_pop(lua_State* L, pbdb::mail& adata, bool apop = true)
       {
           int32_t lmid;
           int32_t lmcreateutc;
           int32_t lmtid;
           std::string lmprams;
           std::string lmcontent;
           bool lmread;
           bool lmdraw;
           if(!nlua_stack::stack_pop(L, lmid, lmcreateutc, lmtid, lmprams, lmcontent,  *adata.mutable_mitems(), lmread, lmdraw))
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::mail& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::mail& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::mailitem>
   {
       static void stack_push(lua_State* L, const pbdb::mailitem& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mitemtid(),adata.mcount());
       }
       static bool stack_pop(lua_State* L, pbdb::mailitem& adata, bool apop = true)
       {
           int32_t lmitemtid;
           int32_t lmcount;
           if(!nlua_stack::stack_pop(L, lmitemtid, lmcount))
           {
               return false;
           }
           adata.set_mitemtid(lmitemtid);
           adata.set_mcount(lmcount);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::mailitem& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::mailitem& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbdb::rankitem>
   {
       static void stack_push(lua_State* L, const pbdb::rankitem& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mvalue(),adata.mtime());
       }
       static bool stack_pop(lua_State* L, pbdb::rankitem& adata, bool apop = true)
       {
           int64_t lmvalue;
           int32_t lmtime;
           if(!nlua_stack::stack_pop(L, lmvalue, lmtime))
           {
               return false;
           }
           adata.set_mvalue(lmvalue);
           adata.set_mtime(lmtime);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbdb::rankitem& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbdb::rankitem& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
}//namespace ngl
