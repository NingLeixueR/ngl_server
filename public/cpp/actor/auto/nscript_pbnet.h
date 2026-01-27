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
 // 创建时间【2026-01-27 15:44:16】

#pragma once
#include "ndefine.h"
#include "example.pb.h"
#include "net.pb.h"
#include "db.pb.h"

#include <array>

namespace ngl
{
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ACOUNT_LOGIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"marea","maccount","mpassword"} , adata.marea(),adata.maccount(),adata.mpassword()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmarea;
           std::string lmaccount;
           std::string lmpassword;
           if(!ngl::nlua_table::table_pop(L, aname, { "marea", "maccount", "mpassword"}, lmarea, lmaccount, lmpassword))
           {
               return false;
           }
           adata.set_marea(lmarea);
           adata.set_maccount(lmaccount);
           adata.set_mpassword(lmpassword);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"marea","mroleid","msession","maccount","mgatewayid","mgameid"} , adata.marea(),adata.mroleid(),adata.msession(),adata.maccount(),adata.mgatewayid(),adata.mgameid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmarea;
           int64_t lmroleid;
           std::string lmsession;
           std::string lmaccount;
           int32_t lmgatewayid;
           int32_t lmgameid;
           if(!ngl::nlua_table::table_pop(L, aname, { "marea", "mroleid", "msession", "maccount", "mgatewayid", "mgameid"}, lmarea, lmroleid, lmsession, lmaccount, lmgatewayid, lmgameid))
           {
               return false;
           }
           adata.set_marea(lmarea);
           adata.set_mroleid(lmroleid);
           adata.set_msession(lmsession);
           adata.set_maccount(lmaccount);
           adata.set_mgatewayid(lmgatewayid);
           adata.set_mgameid(lmgameid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_SYNC>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_SYNC& adata, const char* aname = nullptr)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_SYNC& adata, bool apop = true, const char* aname = nullptr)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_SYNC& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_SYNC& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mbag"} , adata.mbag()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mbag"},  *adata.mutable_mbag()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mdelitems","mdelnostackitems","madditems","maddnostackitems"} , adata.mdelitems(),adata.mdelnostackitems(),adata.madditems(),adata.maddnostackitems()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mdelitems", "mdelnostackitems", "madditems", "maddnostackitems"},  *adata.mutable_mdelitems(),  *adata.mutable_mdelnostackitems(),  *adata.mutable_madditems(),  *adata.mutable_maddnostackitems()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_UPDATE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE::additem>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mcount"} , adata.mid(),adata.mcount()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmid;
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mcount"}, lmid, lmcount))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mcount(lmcount);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE::delitem>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mcount"} , adata.mid(),adata.mcount()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmid;
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mcount"}, lmid, lmcount))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mcount(lmcount);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_CHANGE_ANGLE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHANGE_ANGLE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mposition"} , adata.mposition()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHANGE_ANGLE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mposition"},  *adata.mutable_mposition()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_CHANGE_ANGLE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHANGE_ANGLE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_CHAT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHAT& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtype","mchannelid","mcontent"} , adata.mtype(),adata.mchannelid(),adata.mcontent()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHAT& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmtype;
           int32_t lmchannelid;
           std::string lmcontent;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtype", "mchannelid", "mcontent"}, lmtype, lmchannelid, lmcontent))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mchannelid(lmchannelid);
           adata.set_mcontent(lmcontent);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_CHAT& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHAT& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_CHAT_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHAT_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtype","mchannelid","mchatlist","mstat"} , adata.mtype(),adata.mchannelid(),adata.mchatlist(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHAT_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmtype;
           int32_t lmchannelid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtype", "mchannelid", "mchatlist", "mstat"}, lmtype, lmchannelid,  *adata.mutable_mchatlist(), lmstat))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mchannelid(lmchannelid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_CHAT_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHAT_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_CMD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CMD& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mcmd"} , adata.mcmd()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CMD& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           std::string lmcmd;
           if(!ngl::nlua_table::table_pop(L, aname, { "mcmd"}, lmcmd))
           {
               return false;
           }
           adata.set_mcmd(lmcmd);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_CMD& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CMD& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mrechargeid","morderid","mgold","mitems"} , adata.mrechargeid(),adata.morderid(),adata.mgold(),adata.mitems()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmrechargeid;
           std::string lmorderid;
           int32_t lmgold;
           if(!ngl::nlua_table::table_pop(L, aname, { "mrechargeid", "morderid", "mgold", "mitems"}, lmrechargeid, lmorderid, lmgold,  *adata.mutable_mitems()))
           {
               return false;
           }
           adata.set_mrechargeid(lmrechargeid);
           adata.set_morderid(lmorderid);
           adata.set_mgold(lmgold);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"misenter","munits"} , adata.misenter(),adata.munits()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           bool lmisenter;
           if(!ngl::nlua_table::table_pop(L, aname, { "misenter", "munits"}, lmisenter,  *adata.mutable_munits()))
           {
               return false;
           }
           adata.set_misenter(lmisenter);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ERROR>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ERROR& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"merrmessage"} , adata.merrmessage()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ERROR& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           std::string lmerrmessage;
           if(!ngl::nlua_table::table_pop(L, aname, { "merrmessage"}, lmerrmessage))
           {
               return false;
           }
           adata.set_merrmessage(lmerrmessage);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ERROR& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ERROR& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CEDE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CEDE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mroleid"} , adata.mroleid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CEDE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmroleid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mroleid"}, lmroleid))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CEDE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CEDE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat"} , adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfamilid","mname"} , adata.mfamilid(),adata.mname()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfamilid;
           std::string lmname;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfamilid", "mname"}, lmfamilid, lmname))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           adata.set_mname(lmname);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat"} , adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CREATE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CREATE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mname"} , adata.mname()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CREATE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           std::string lmname;
           if(!ngl::nlua_table::table_pop(L, aname, { "mname"}, lmname))
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat"} , adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_INFO>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_INFO& adata, const char* aname = nullptr)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_INFO& adata, bool apop = true, const char* aname = nullptr)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_INFO& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_INFO& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat","minfo","mmember"} , adata.mstat(),adata.minfo(),adata.mmember()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mstat", "minfo", "mmember"}, lmstat,  *adata.mutable_minfo(),  *adata.mutable_mmember()))
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_JOIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_JOIN& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfamilid","mapply"} , adata.mfamilid(),adata.mapply()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_JOIN& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfamilid;
           bool lmapply;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfamilid", "mapply"}, lmfamilid, lmapply))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           adata.set_mapply(lmapply);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_JOIN& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_JOIN& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat"} , adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LEAVE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LEAVE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfamilid"} , adata.mfamilid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LEAVE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfamilid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfamilid"}, lmfamilid))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LEAVE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LEAVE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat"} , adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LIST>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LIST& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfamilid"} , adata.mfamilid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LIST& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfamilid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfamilid"}, lmfamilid))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LIST& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LIST& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfamilid","mfamily"} , adata.mfamilid(),adata.mfamily()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfamilid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfamilid", "mfamily"}, lmfamilid,  *adata.mutable_mfamily()))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mroleid","mratify"} , adata.mroleid(),adata.mratify()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmroleid;
           bool lmratify;
           if(!ngl::nlua_table::table_pop(L, aname, { "mroleid", "mratify"}, lmroleid, lmratify))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           adata.set_mratify(lmratify);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat"} , adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_SIGN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_SIGN& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfamilid"} , adata.mfamilid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_SIGN& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfamilid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfamilid"}, lmfamilid))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_SIGN& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_SIGN& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat"} , adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND& adata, const char* aname = nullptr)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND& adata, bool apop = true, const char* aname = nullptr)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ADD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ADD& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfriedid"} , adata.mfriedid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ADD& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfriedid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfriedid"}, lmfriedid))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_ADD& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ADD& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfriedid","mstat"} , adata.mfriedid(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfriedid;
           int32_t lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfriedid", "mstat"}, lmfriedid, lmstat))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ERASE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ERASE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfriedid"} , adata.mfriedid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ERASE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfriedid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfriedid"}, lmfriedid))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_ERASE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ERASE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfriedid","mstat"} , adata.mfriedid(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfriedid;
           int32_t lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfriedid", "mstat"}, lmfriedid, lmstat))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfriedid","mratify"} , adata.mfriedid(),adata.mratify()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfriedid;
           bool lmratify;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfriedid", "mratify"}, lmfriedid, lmratify))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           adata.set_mratify(lmratify);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfriedid","mratify","mstat"} , adata.mfriedid(),adata.mratify(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmfriedid;
           bool lmratify;
           int32_t lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mfriedid", "mratify", "mstat"}, lmfriedid, lmratify, lmstat))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           adata.set_mratify(lmratify);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mfriends","mapplyfriends"} , adata.mfriends(),adata.mapplyfriends()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mfriends", "mapplyfriends"},  *adata.mutable_mfriends(),  *adata.mutable_mapplyfriends()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_GET_TIME>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_GET_TIME& adata, const char* aname = nullptr)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_GET_TIME& adata, bool apop = true, const char* aname = nullptr)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_GET_TIME& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_GET_TIME& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mutc"} , adata.mutc()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmutc;
           if(!ngl::nlua_table::table_pop(L, aname, { "mutc"}, lmutc))
           {
               return false;
           }
           adata.set_mutc(lmutc);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_HEARTBEAT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_HEARTBEAT& adata, const char* aname = nullptr)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_HEARTBEAT& adata, bool apop = true, const char* aname = nullptr)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_HEARTBEAT& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_HEARTBEAT& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_KCPSESSION>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_KCPSESSION& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mserverid","muip","muport","mconv","mactoridserver","mactoridclient","m_kcpnum"} , adata.mserverid(),adata.muip(),adata.muport(),adata.mconv(),adata.mactoridserver(),adata.mactoridclient(),adata.m_kcpnum()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_KCPSESSION& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmserverid;
           std::string lmuip;
           int32_t lmuport;
           int32_t lmconv;
           int64_t lmactoridserver;
           int64_t lmactoridclient;
           pbnet::ENUM_KCP lm_kcpnum;
           if(!ngl::nlua_table::table_pop(L, aname, { "mserverid", "muip", "muport", "mconv", "mactoridserver", "mactoridclient", "m_kcpnum"}, lmserverid, lmuip, lmuport, lmconv, lmactoridserver, lmactoridclient, lm_kcpnum))
           {
               return false;
           }
           adata.set_mserverid(lmserverid);
           adata.set_muip(lmuip);
           adata.set_muport(lmuport);
           adata.set_mconv(lmconv);
           adata.set_mactoridserver(lmactoridserver);
           adata.set_mactoridclient(lmactoridclient);
           adata.set_m_kcpnum(lm_kcpnum);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_KCPSESSION& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_KCPSESSION& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mkcpsession","mserverid","m_kcpnum","mactoridserver","mactoridclient"} , adata.mkcpsession(),adata.mserverid(),adata.m_kcpnum(),adata.mactoridserver(),adata.mactoridclient()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           std::string lmkcpsession;
           int64_t lmserverid;
           pbnet::ENUM_KCP lm_kcpnum;
           int64_t lmactoridserver;
           int64_t lmactoridclient;
           if(!ngl::nlua_table::table_pop(L, aname, { "mkcpsession", "mserverid", "m_kcpnum", "mactoridserver", "mactoridclient"}, lmkcpsession, lmserverid, lm_kcpnum, lmactoridserver, lmactoridclient))
           {
               return false;
           }
           adata.set_mkcpsession(lmkcpsession);
           adata.set_mserverid(lmserverid);
           adata.set_m_kcpnum(lm_kcpnum);
           adata.set_mactoridserver(lmactoridserver);
           adata.set_mactoridclient(lmactoridclient);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DEL>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DEL& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmailid"} , adata.mmailid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DEL& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmmailid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmailid"}, lmmailid))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_DEL& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DEL& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmailid","mstat"} , adata.mmailid(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmmailid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmailid", "mstat"}, lmmailid, lmstat))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DRAW>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DRAW& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmailid"} , adata.mmailid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DRAW& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmmailid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmailid"}, lmmailid))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_DRAW& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DRAW& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmailid","mstat"} , adata.mmailid(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmmailid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmailid", "mstat"}, lmmailid, lmstat))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_LIST>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_LIST& adata, const char* aname = nullptr)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_LIST& adata, bool apop = true, const char* aname = nullptr)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_LIST& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_LIST& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmail"} , adata.mmail()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mmail"},  *adata.mutable_mmail()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_READ>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_READ& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmailid"} , adata.mmailid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_READ& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmmailid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmailid"}, lmmailid))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_READ& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_READ& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmailid","mstat"} , adata.mmailid(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmmailid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmailid", "mstat"}, lmmailid, lmstat))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MSG_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MSG_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmsg"} , adata.mmsg()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MSG_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           std::string lmmsg;
           if(!ngl::nlua_table::table_pop(L, aname, { "mmsg"}, lmmsg))
           {
               return false;
           }
           adata.set_mmsg(lmmsg);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MSG_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MSG_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_NOTICE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_NOTICE& adata, const char* aname = nullptr)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_NOTICE& adata, bool apop = true, const char* aname = nullptr)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_NOTICE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_NOTICE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_NOTICE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mnotices"} , adata.mnotices()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mnotices"},  *adata.mutable_mnotices()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_RANKLIST>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RANKLIST& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"miscross","mtype","mactivityid","mpage"} , adata.miscross(),adata.mtype(),adata.mactivityid(),adata.mpage()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RANKLIST& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           bool lmiscross;
           pbdb::eranklist lmtype;
           int32_t lmactivityid;
           int32_t lmpage;
           if(!ngl::nlua_table::table_pop(L, aname, { "miscross", "mtype", "mactivityid", "mpage"}, lmiscross, lmtype, lmactivityid, lmpage))
           {
               return false;
           }
           adata.set_miscross(lmiscross);
           adata.set_mtype(lmtype);
           adata.set_mactivityid(lmactivityid);
           adata.set_mpage(lmpage);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_RANKLIST& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RANKLIST& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtype","mitems","mpage","mcount","mrolerank"} , adata.mtype(),adata.mitems(),adata.mpage(),adata.mcount(),adata.mrolerank()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbdb::eranklist lmtype;
           int32_t lmpage;
           int32_t lmcount;
           int32_t lmrolerank;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtype", "mitems", "mpage", "mcount", "mrolerank"}, lmtype,  *adata.mutable_mitems(), lmpage, lmcount, lmrolerank))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mpage(lmpage);
           adata.set_mcount(lmcount);
           adata.set_mrolerank(lmrolerank);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_RECHARGE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RECHARGE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mrechargeid"} , adata.mrechargeid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RECHARGE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmrechargeid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mrechargeid"}, lmrechargeid))
           {
               return false;
           }
           adata.set_mrechargeid(lmrechargeid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_RECHARGE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RECHARGE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mrechargeid","mstat","morderid"} , adata.mrechargeid(),adata.mstat(),adata.morderid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmrechargeid;
           pbnet::PROBUFF_NET_RECHARGE_RESPONSE::Estat lmstat;
           std::string lmorderid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mrechargeid", "mstat", "morderid"}, lmrechargeid, lmstat, lmorderid))
           {
               return false;
           }
           adata.set_mrechargeid(lmrechargeid);
           adata.set_mstat(lmstat);
           adata.set_morderid(lmorderid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mitems"} , adata.mitems()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mitems"},  *adata.mutable_mitems()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLESTAT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLESTAT& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat","mlogicstat","mroleid"} , adata.mstat(),adata.mlogicstat(),adata.mroleid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLESTAT& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbnet::PROBUFF_NET_ROLESTAT::stat lmstat;
           pbnet::PROBUFF_NET_ROLESTAT::logic_stat lmlogicstat;
           int64_t lmroleid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mstat", "mlogicstat", "mroleid"}, lmstat, lmlogicstat, lmroleid))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           adata.set_mlogicstat(lmlogicstat);
           adata.set_mroleid(lmroleid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLESTAT& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLESTAT& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_CREATE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_CREATE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mname"} , adata.mname()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_CREATE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           std::string lmname;
           if(!ngl::nlua_table::table_pop(L, aname, { "mname"}, lmname))
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_CREATE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_CREATE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_CREATE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_CREATE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mstat","mname"} , adata.mstat(),adata.mname()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_CREATE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           pbnet::PROBUFF_NET_ROLE_CREATE_RESPONSE::estat lmstat;
           std::string lmname;
           if(!ngl::nlua_table::table_pop(L, aname, { "mstat", "mname"}, lmstat, lmname))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           adata.set_mname(lmname);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_CREATE_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_CREATE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_LOGIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_LOGIN& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mroleid","msession","marea","mgatewayid","mgameid"} , adata.mroleid(),adata.msession(),adata.marea(),adata.mgatewayid(),adata.mgameid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_LOGIN& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmroleid;
           std::string lmsession;
           int32_t lmarea;
           int32_t lmgatewayid;
           int32_t lmgameid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mroleid", "msession", "marea", "mgatewayid", "mgameid"}, lmroleid, lmsession, lmarea, lmgatewayid, lmgameid))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           adata.set_msession(lmsession);
           adata.set_marea(lmarea);
           adata.set_mgatewayid(lmgatewayid);
           adata.set_mgameid(lmgameid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_NOT_CREATE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_NOT_CREATE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mroleid"} , adata.mroleid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_NOT_CREATE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmroleid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mroleid"}, lmroleid))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_NOT_CREATE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_NOT_CREATE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_SYNC>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_SYNC& adata, const char* aname = nullptr)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_SYNC& adata, bool apop = true, const char* aname = nullptr)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_SYNC& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_SYNC& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mrole","mbrief","mbag","mtask"} , adata.mrole(),adata.mbrief(),adata.mbag(),adata.mtask()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mrole", "mbrief", "mbag", "mtask"},  *adata.mutable_mrole(),  *adata.mutable_mbrief(),  *adata.mutable_mbag(),  *adata.mutable_mtask()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SWITCH_LINE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SWITCH_LINE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mline"} , adata.mline()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SWITCH_LINE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmline;
           if(!ngl::nlua_table::table_pop(L, aname, { "mline"}, lmline))
           {
               return false;
           }
           adata.set_mline(lmline);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SWITCH_LINE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SWITCH_LINE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mline","mstat"} , adata.mline(),adata.mstat()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmline;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mline", "mstat"}, lmline, lmstat))
           {
               return false;
           }
           adata.set_mline(lmline);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mmodules"} , adata.mmodules()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mmodules"},  *adata.mutable_mmodules()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_POSITION>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_POSITION& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mposition"} , adata.mposition()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_POSITION& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "mposition"},  *adata.mutable_mposition()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SYNC_POSITION& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_POSITION& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_UNIT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_UNIT& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"munits"} , adata.munits()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_UNIT& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           if(!ngl::nlua_table::table_pop(L, aname, { "munits"},  *adata.mutable_munits()))
           {
               return false;
           }
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SYNC_UNIT& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_UNIT& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtaskid"} , adata.mtaskid()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmtaskid;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtaskid"}, lmtaskid))
           {
               return false;
           }
           adata.set_mtaskid(lmtaskid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mtaskid","mstat","mdrop"} , adata.mtaskid(),adata.mstat(),adata.mdrop()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmtaskid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, aname, { "mtaskid", "mstat", "mdrop"}, lmtaskid, lmstat,  *adata.mutable_mdrop()))
           {
               return false;
           }
           adata.set_mtaskid(lmtaskid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_TESTLUA>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TESTLUA& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mid","mvalue","mdata"} , adata.mid(),adata.mvalue(),adata.mdata()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TESTLUA& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmid;
           std::string lmvalue;
           if(!ngl::nlua_table::table_pop(L, aname, { "mid", "mvalue", "mdata"}, lmid, lmvalue,  *adata.mutable_mdata()))
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_TESTLUA& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TESTLUA& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::chatitem>
   {
       static void stack_push(lua_State* L, const pbnet::chatitem& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mroleid","mrolename","mcontent","mutc"} , adata.mroleid(),adata.mrolename(),adata.mcontent(),adata.mutc()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::chatitem& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int64_t lmroleid;
           std::string lmrolename;
           std::string lmcontent;
           int32_t lmutc;
           if(!ngl::nlua_table::table_pop(L, aname, { "mroleid", "mrolename", "mcontent", "mutc"}, lmroleid, lmrolename, lmcontent, lmutc))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           adata.set_mrolename(lmrolename);
           adata.set_mcontent(lmcontent);
           adata.set_mutc(lmutc);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::chatitem& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::chatitem& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
   template <>
   struct serialize_lua<pbnet::drop_item>
   {
       static void stack_push(lua_State* L, const pbnet::drop_item& adata, const char* aname = nullptr)
       {
            ngl::nlua_table::table_push(L, aname, {"mitem","mcount"} , adata.mitem(),adata.mcount()
               );
       }
       static bool stack_pop(lua_State* L, pbnet::drop_item& adata, bool apop = true, const char* aname = nullptr)
       {
         if (lua_isnil(L, -1))
            {
            return true;
            }
           int32_t lmitem;
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, aname, { "mitem", "mcount"}, lmitem, lmcount))
           {
               return false;
           }
           adata.set_mitem(lmitem);
           adata.set_mcount(lmcount);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::drop_item& adata)
       {
           stack_push(L, adata, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::drop_item& adata)
       {
           return stack_pop(L, adata, false, aname);
       }
   };
}//namespace ngl
