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
   struct serialize_lua<pbexample::PLAYER>
   {
       static void stack_push(lua_State* L, const pbexample::PLAYER& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mplayers(),adata.misconfirm());
       }
       static bool stack_pop(lua_State* L, pbexample::PLAYER& adata, bool apop = true)
       {
           bool lmisconfirm;
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mplayers(), lmisconfirm))
           {
               return false;
           }
           adata.set_misconfirm(lmisconfirm);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PLAYER& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PLAYER& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mguessnumber());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata, bool apop = true)
       {
           int32_t lmguessnumber;
           if(!nlua_stack::stack_pop(L, lmguessnumber))
           {
               return false;
           }
           adata.set_mguessnumber(lmguessnumber);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mbombvalue(),adata.mroleid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata, bool apop = true)
       {
           int32_t lmbombvalue;
           int64_t lmroleid;
           if(!nlua_stack::stack_pop(L, lmbombvalue, lmroleid))
           {
               return false;
           }
           adata.set_mbombvalue(lmbombvalue);
           adata.set_mroleid(lmroleid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata, bool apop = true)
       {
           pbexample::E_GUESS_NUMBER_ERROR lmstat;
           if(!nlua_stack::stack_pop(L, lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmaxnumber(),adata.mminnumber(),adata.mwhoguess());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata, bool apop = true)
       {
           int32_t lmmaxnumber;
           int32_t lmminnumber;
           int64_t lmwhoguess;
           if(!nlua_stack::stack_pop(L, lmmaxnumber, lmminnumber, lmwhoguess))
           {
               return false;
           }
           adata.set_mmaxnumber(lmmaxnumber);
           adata.set_mminnumber(lmminnumber);
           adata.set_mwhoguess(lmwhoguess);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtype(),adata.mroomid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata, bool apop = true)
       {
           pbexample::EPLAY_TYPE lmtype;
           int32_t lmroomid;
           if(!nlua_stack::stack_pop(L, lmtype, lmroomid))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mroomid(lmroomid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtype(),adata.mexampleactorid(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata, bool apop = true)
       {
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           pbexample::PROBUFF_EXAMPLE_PLAY_CREATE::estat lmstat;
           if(!nlua_stack::stack_pop(L, lmtype, lmexampleactorid, lmstat))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mexampleactorid(lmexampleactorid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mcross(),adata.mtype(),adata.mexampleactorid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata, bool apop = true)
       {
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           if(!nlua_stack::stack_pop(L, lmcross, lmtype, lmexampleactorid))
           {
               return false;
           }
           adata.set_mcross(lmcross);
           adata.set_mtype(lmtype);
           adata.set_mexampleactorid(lmexampleactorid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mcross(),adata.mtype(),adata.mexampleactorid(),adata.mplayers());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata, bool apop = true)
       {
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           if(!nlua_stack::stack_pop(L, lmcross, lmtype, lmexampleactorid,  *adata.mutable_mplayers()))
           {
               return false;
           }
           adata.set_mcross(lmcross);
           adata.set_mtype(lmtype);
           adata.set_mexampleactorid(lmexampleactorid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mcross(),adata.mtype());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata, bool apop = true)
       {
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           if(!nlua_stack::stack_pop(L, lmcross, lmtype))
           {
               return false;
           }
           adata.set_mcross(lmcross);
           adata.set_mtype(lmtype);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mroomid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata, bool apop = true)
       {
           int32_t lmroomid;
           if(!nlua_stack::stack_pop(L, lmroomid))
           {
               return false;
           }
           adata.set_mroomid(lmroomid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mroomid(),adata.mtotalnumber(),adata.mroomcreate(),adata.mroomready(),adata.mplayers());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata, bool apop = true)
       {
           int32_t lmroomid;
           int32_t lmtotalnumber;
           int64_t lmroomcreate;
           int64_t lmroomready;
           if(!nlua_stack::stack_pop(L, lmroomid, lmtotalnumber, lmroomcreate, lmroomready,  *adata.mutable_mplayers()))
           {
               return false;
           }
           adata.set_mroomid(lmroomid);
           adata.set_mtotalnumber(lmtotalnumber);
           adata.set_mroomcreate(lmroomcreate);
           adata.set_mroomready(lmroomready);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.merrorcode(),adata.mroomid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata, bool apop = true)
       {
           pbexample::PLAY_MATCHING_EERROR_CODE lmerrorcode;
           int32_t lmroomid;
           if(!nlua_stack::stack_pop(L, lmerrorcode, lmroomid))
           {
               return false;
           }
           adata.set_merrorcode(lmerrorcode);
           adata.set_mroomid(lmroomid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.misconfirm(),adata.mroomid());
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata, bool apop = true)
       {
           bool lmisconfirm;
           int32_t lmroomid;
           if(!nlua_stack::stack_pop(L, lmisconfirm, lmroomid))
           {
               return false;
           }
           adata.set_misconfirm(lmisconfirm);
           adata.set_mroomid(lmroomid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
}//namespace ngl
