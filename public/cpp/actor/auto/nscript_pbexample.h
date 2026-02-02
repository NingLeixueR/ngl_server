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
 // 创建时间【2026-02-02 18:27:57】

#pragma once
#include "ndefine.h"
#include "example.pb.h"
#include "net.pb.h"
#include "db.pb.h"

#include <array>

namespace ngl
{
   template <>
   struct serialize_lua<pbexample::PLAYER>
   {
       static void stack_push(lua_State* L, const pbexample::PLAYER& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mplayers","misconfirm"} , adata.mplayers(),adata.misconfirm()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PLAYER& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           bool lmisconfirm;
           if(!ngl::nlua_table::table_pop(L, aname, { "mplayers", "misconfirm"},  *adata.mutable_mplayers(), lmisconfirm))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PLAYER& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mguessnumber"} , adata.mguessnumber()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmguessnumber;
           if(!ngl::nlua_table::table_pop(L, aname, { "mguessnumber"}, lmguessnumber))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mbombvalue","mroleid"} , adata.mbombvalue(),adata.mroleid()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmbombvalue;
           int64_t lmroleid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mbombvalue", "mroleid"}, lmbombvalue, lmroleid))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat"} , adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbexample::E_GUESS_NUMBER_ERROR lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mstat"}, lmstat))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmaxnumber","mminnumber","mwhoguess"} , adata.mmaxnumber(),adata.mminnumber(),adata.mwhoguess()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmmaxnumber;
           int32_t lmminnumber;
           int64_t lmwhoguess;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmaxnumber", "mminnumber", "mwhoguess"}, lmmaxnumber, lmminnumber, lmwhoguess))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtype","mroomid"} , adata.mtype(),adata.mroomid()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbexample::EPLAY_TYPE lmtype;
           int32_t lmroomid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtype", "mroomid"}, lmtype, lmroomid))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtype","mexampleactorid","mstat"} , adata.mtype(),adata.mexampleactorid(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           pbexample::PROBUFF_EXAMPLE_PLAY_CREATE::estat lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtype", "mexampleactorid", "mstat"}, lmtype, lmexampleactorid, lmstat))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_CREATE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mcross","mtype","mexampleactorid"} , adata.mcross(),adata.mtype(),adata.mexampleactorid()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mcross", "mtype", "mexampleactorid"}, lmcross, lmtype, lmexampleactorid))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mcross","mtype","mexampleactorid","mplayers"} , adata.mcross(),adata.mtype(),adata.mexampleactorid(),adata.mplayers()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           int64_t lmexampleactorid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mcross", "mtype", "mexampleactorid", "mplayers"}, lmcross, lmtype, lmexampleactorid,  *adata.mutable_mplayers()))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mcross","mtype"} , adata.mcross(),adata.mtype()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbexample::ECROSS lmcross;
           pbexample::EPLAY_TYPE lmtype;
           if(!ngl::nlua_table::table_pop(L, aname, { "mcross", "mtype"}, lmcross, lmtype))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mroomid"} , adata.mroomid()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmroomid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mroomid"}, lmroomid))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mroomid","mtotalnumber","mroomcreate","mroomready","mplayers"} , adata.mroomid(),adata.mtotalnumber(),adata.mroomcreate(),adata.mroomready(),adata.mplayers()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmroomid;
           int32_t lmtotalnumber;
           int64_t lmroomcreate;
           int64_t lmroomready;
           if(!ngl::nlua_table::table_pop(L, aname, { "mroomid", "mtotalnumber", "mroomcreate", "mroomready", "mplayers"}, lmroomid, lmtotalnumber, lmroomcreate, lmroomready,  *adata.mutable_mplayers()))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"merrorcode","mroomid"} , adata.merrorcode(),adata.mroomid()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbexample::PLAY_MATCHING_EERROR_CODE lmerrorcode;
           int32_t lmroomid;
           if(!ngl::nlua_table::table_pop(L, aname, { "merrorcode", "mroomid"}, lmerrorcode, lmroomid))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>
   {
       static void stack_push(lua_State* L, const pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"misconfirm","mroomid"} , adata.misconfirm(),adata.mroomid()
               );
       }
       static bool stack_pop(lua_State* L, pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           bool lmisconfirm;
           int32_t lmroomid;
           if(!ngl::nlua_table::table_pop(L, aname, { "misconfirm", "mroomid"}, lmisconfirm, lmroomid))
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
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
}//namespace ngl
