 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2025-08-20 09:44:01】

#pragma once
#include "ndefine.h"
#include "example.pb.h"
#include "net.pb.h"
#include "db.pb.h"

namespace ngl
{
   template <>
   struct serialize_lua<pbexample::PLAYER>
   {
       static void stack_push(lua_State* L, const pbexample::PLAYER& adata)
       {
            ngl::nlua_table::table_push(L, "mplayers", adata.mplayers(),"misconfirm", adata.misconfirm());
       }
       static bool stack_pop(lua_State* L, pbexample::PLAYER& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           bool lmisconfirm;
           if(!ngl::nlua_table::table_pop(L, "mplayers",  *adata.mutable_mplayers(), "misconfirm", lmisconfirm))
           {
               return false;
           }
           adata.set_misconfirm(lmisconfirm);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PLAYER& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PLAYER& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
       {
            ngl::nlua_table::table_push(L, "mguessnumber", adata.mguessnumber());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmguessnumber;
           if(!ngl::nlua_table::table_pop(L, "mguessnumber", lmguessnumber))
           {
               return false;
           }
           adata.set_mguessnumber(lmguessnumber);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata)
       {
            ngl::nlua_table::table_push(L, "mbombvalue", adata.mbombvalue(),"mroleid", adata.mroleid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmbombvalue;
           int64_t lmroleid;
           if(!ngl::nlua_table::table_pop(L, "mbombvalue", lmbombvalue, "mroleid", lmroleid))
           {
               return false;
           }
           adata.set_mbombvalue(lmbombvalue);
           adata.set_mroleid(lmroleid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbexample::E_GUESS_NUMBER_ERROR lmstat;
           if(!ngl::nlua_table::table_pop(L, "mstat", lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata)
       {
            ngl::nlua_table::table_push(L, "mmaxnumber", adata.mmaxnumber(),"mminnumber", adata.mminnumber(),"mwhoguess", adata.mwhoguess());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmmaxnumber;
           int32_t lmminnumber;
           int64_t lmwhoguess;
           if(!ngl::nlua_table::table_pop(L, "mmaxnumber", lmmaxnumber, "mminnumber", lmminnumber, "mwhoguess", lmwhoguess))
           {
               return false;
           }
           adata.set_mmaxnumber(lmmaxnumber);
           adata.set_mminnumber(lmminnumber);
           adata.set_mwhoguess(lmwhoguess);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata)
       {
            ngl::nlua_table::table_push(L, "mtype", adata.mtype(),"mroomid", adata.mroomid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbexample::EPLAY_TYPE lmtype;
           int32_t lmroomid;
           if(!ngl::nlua_table::table_pop(L, "mtype", lmtype, "mroomid", lmroomid))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mroomid(lmroomid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata)
       {
            ngl::nlua_table::table_push(L, "mtype", adata.mtype(),"mexampleactorid", adata.mexampleactorid(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           pbexample::PROBUFF_EXAMPLE_PLAY_CREATE::estat lmstat;
           if(!ngl::nlua_table::table_pop(L, "mtype", lmtype, "mexampleactorid", lmexampleactorid, "mstat", lmstat))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mexampleactorid(lmexampleactorid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
       {
            ngl::nlua_table::table_push(L, "mcross", adata.mcross(),"mtype", adata.mtype(),"mexampleactorid", adata.mexampleactorid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           if(!ngl::nlua_table::table_pop(L, "mcross", lmcross, "mtype", lmtype, "mexampleactorid", lmexampleactorid))
           {
               return false;
           }
           adata.set_mcross(lmcross);
           adata.set_mtype(lmtype);
           adata.set_mexampleactorid(lmexampleactorid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mcross", adata.mcross(),"mtype", adata.mtype(),"mexampleactorid", adata.mexampleactorid(),"mplayers", adata.mplayers());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           if(!ngl::nlua_table::table_pop(L, "mcross", lmcross, "mtype", lmtype, "mexampleactorid", lmexampleactorid, "mplayers",  *adata.mutable_mplayers()))
           {
               return false;
           }
           adata.set_mcross(lmcross);
           adata.set_mtype(lmtype);
           adata.set_mexampleactorid(lmexampleactorid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
       {
            ngl::nlua_table::table_push(L, "mcross", adata.mcross(),"mtype", adata.mtype());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           if(!ngl::nlua_table::table_pop(L, "mcross", lmcross, "mtype", lmtype))
           {
               return false;
           }
           adata.set_mcross(lmcross);
           adata.set_mtype(lmtype);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mroomid", adata.mroomid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmroomid;
           if(!ngl::nlua_table::table_pop(L, "mroomid", lmroomid))
           {
               return false;
           }
           adata.set_mroomid(lmroomid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata)
       {
            ngl::nlua_table::table_push(L, "mroomid", adata.mroomid(),"mtotalnumber", adata.mtotalnumber(),"mroomcreate", adata.mroomcreate(),"mroomready", adata.mroomready(),"mplayers", adata.mplayers());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmroomid;
           int32_t lmtotalnumber;
           int64_t lmroomcreate;
           int64_t lmroomready;
           if(!ngl::nlua_table::table_pop(L, "mroomid", lmroomid, "mtotalnumber", lmtotalnumber, "mroomcreate", lmroomcreate, "mroomready", lmroomready, "mplayers",  *adata.mutable_mplayers()))
           {
               return false;
           }
           adata.set_mroomid(lmroomid);
           adata.set_mtotalnumber(lmtotalnumber);
           adata.set_mroomcreate(lmroomcreate);
           adata.set_mroomready(lmroomready);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata)
       {
            ngl::nlua_table::table_push(L, "merrorcode", adata.merrorcode(),"mroomid", adata.mroomid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbexample::PLAY_MATCHING_EERROR_CODE lmerrorcode;
           int32_t lmroomid;
           if(!ngl::nlua_table::table_pop(L, "merrorcode", lmerrorcode, "mroomid", lmroomid))
           {
               return false;
           }
           adata.set_merrorcode(lmerrorcode);
           adata.set_mroomid(lmroomid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata)
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
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata)
       {
            ngl::nlua_table::table_push(L, "misconfirm", adata.misconfirm(),"mroomid", adata.mroomid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           bool lmisconfirm;
           int32_t lmroomid;
           if(!ngl::nlua_table::table_pop(L, "misconfirm", lmisconfirm, "mroomid", lmroomid))
           {
               return false;
           }
           adata.set_misconfirm(lmisconfirm);
           adata.set_mroomid(lmroomid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata)
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
