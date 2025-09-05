 // ע�⡾makeproto ���������ļ�����Ҫ�ֶ��޸ġ�
 // ����ʱ�䡾2025-09-05 17:30:32��

#pragma once
#include "ndefine.h"
#include "example.pb.h"
#include "net.pb.h"
#include "db.pb.h"

namespace ngl
{
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ACOUNT_LOGIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata)
       {
            ngl::nlua_table::table_push(L, "marea", adata.marea(),"maccount", adata.maccount(),"mpassword", adata.mpassword());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmarea;
           std::string lmaccount;
           std::string lmpassword;
           if(!ngl::nlua_table::table_pop(L, "marea", lmarea, "maccount", lmaccount, "mpassword", lmpassword))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "marea", adata.marea(),"mroleid", adata.mroleid(),"msession", adata.msession(),"maccount", adata.maccount(),"mgatewayid", adata.mgatewayid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmarea;
           int64_t lmroleid;
           std::string lmsession;
           std::string lmaccount;
           int32_t lmgatewayid;
           if(!ngl::nlua_table::table_pop(L, "marea", lmarea, "mroleid", lmroleid, "msession", lmsession, "maccount", lmaccount, "mgatewayid", lmgatewayid))
           {
               return false;
           }
           adata.set_marea(lmarea);
           adata.set_mroleid(lmroleid);
           adata.set_msession(lmsession);
           adata.set_maccount(lmaccount);
           adata.set_mgatewayid(lmgatewayid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_SYNC>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_SYNC& adata)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_SYNC& adata, bool apop = true)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_SYNC& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_SYNC& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mbag", adata.mbag());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mbag",  *adata.mutable_mbag()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE& adata)
       {
            ngl::nlua_table::table_push(L, "mdelitems", adata.mdelitems(),"mdelnostackitems", adata.mdelnostackitems(),"madditems", adata.madditems(),"maddnostackitems", adata.maddnostackitems());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mdelitems",  *adata.mutable_mdelitems(), "mdelnostackitems",  *adata.mutable_mdelnostackitems(), "madditems",  *adata.mutable_madditems(), "maddnostackitems",  *adata.mutable_maddnostackitems()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE::additem>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mcount", adata.mcount());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmid;
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mcount", lmcount))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE::delitem>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mcount", adata.mcount());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmid;
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mcount", lmcount))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_CHANGE_ANGLE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHANGE_ANGLE& adata)
       {
            ngl::nlua_table::table_push(L, "mposition", adata.mposition());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHANGE_ANGLE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mposition",  *adata.mutable_mposition()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHANGE_ANGLE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_CHAT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHAT& adata)
       {
            ngl::nlua_table::table_push(L, "mtype", adata.mtype(),"mchannelid", adata.mchannelid(),"mcontent", adata.mcontent());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHAT& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmtype;
           int32_t lmchannelid;
           std::string lmcontent;
           if(!ngl::nlua_table::table_pop(L, "mtype", lmtype, "mchannelid", lmchannelid, "mcontent", lmcontent))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHAT& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_CHAT_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHAT_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mtype", adata.mtype(),"mchannelid", adata.mchannelid(),"mchatlist", adata.mchatlist(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHAT_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmtype;
           int32_t lmchannelid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, "mtype", lmtype, "mchannelid", lmchannelid, "mchatlist",  *adata.mutable_mchatlist(), "mstat", lmstat))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHAT_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_CMD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CMD& adata)
       {
            ngl::nlua_table::table_push(L, "mcmd", adata.mcmd());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CMD& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           std::string lmcmd;
           if(!ngl::nlua_table::table_pop(L, "mcmd", lmcmd))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CMD& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata)
       {
            ngl::nlua_table::table_push(L, "mrechargeid", adata.mrechargeid(),"morderid", adata.morderid(),"mgold", adata.mgold(),"mitems", adata.mitems());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmrechargeid;
           std::string lmorderid;
           int32_t lmgold;
           if(!ngl::nlua_table::table_pop(L, "mrechargeid", lmrechargeid, "morderid", lmorderid, "mgold", lmgold, "mitems",  *adata.mutable_mitems()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata)
       {
            ngl::nlua_table::table_push(L, "misenter", adata.misenter(),"munits", adata.munits());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           bool lmisenter;
           if(!ngl::nlua_table::table_pop(L, "misenter", lmisenter, "munits",  *adata.mutable_munits()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_ERROR>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ERROR& adata)
       {
            ngl::nlua_table::table_push(L, "merrmessage", adata.merrmessage());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ERROR& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           std::string lmerrmessage;
           if(!ngl::nlua_table::table_pop(L, "merrmessage", lmerrmessage))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ERROR& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CEDE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CEDE& adata)
       {
            ngl::nlua_table::table_push(L, "mroleid", adata.mroleid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CEDE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmroleid;
           if(!ngl::nlua_table::table_pop(L, "mroleid", lmroleid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CEDE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata)
       {
            ngl::nlua_table::table_push(L, "mfamilid", adata.mfamilid(),"mname", adata.mname());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfamilid;
           std::string lmname;
           if(!ngl::nlua_table::table_pop(L, "mfamilid", lmfamilid, "mname", lmname))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CREATE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
       {
            ngl::nlua_table::table_push(L, "mname", adata.mname());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CREATE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           std::string lmname;
           if(!ngl::nlua_table::table_pop(L, "mname", lmname))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_INFO>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_INFO& adata)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_INFO& adata, bool apop = true)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_INFO& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_INFO& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat(),"minfo", adata.minfo(),"mmember", adata.mmember());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmstat;
           if(!ngl::nlua_table::table_pop(L, "mstat", lmstat, "minfo",  *adata.mutable_minfo(), "mmember",  *adata.mutable_mmember()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_JOIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_JOIN& adata)
       {
            ngl::nlua_table::table_push(L, "mfamilid", adata.mfamilid(),"mapply", adata.mapply());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_JOIN& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfamilid;
           bool lmapply;
           if(!ngl::nlua_table::table_pop(L, "mfamilid", lmfamilid, "mapply", lmapply))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_JOIN& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LEAVE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LEAVE& adata)
       {
            ngl::nlua_table::table_push(L, "mfamilid", adata.mfamilid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LEAVE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfamilid;
           if(!ngl::nlua_table::table_pop(L, "mfamilid", lmfamilid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LEAVE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LIST>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LIST& adata)
       {
            ngl::nlua_table::table_push(L, "mfamilid", adata.mfamilid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LIST& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfamilid;
           if(!ngl::nlua_table::table_pop(L, "mfamilid", lmfamilid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LIST& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mfamilid", adata.mfamilid(),"mfamily", adata.mfamily());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfamilid;
           if(!ngl::nlua_table::table_pop(L, "mfamilid", lmfamilid, "mfamily",  *adata.mutable_mfamily()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata)
       {
            ngl::nlua_table::table_push(L, "mroleid", adata.mroleid(),"mratify", adata.mratify());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmroleid;
           bool lmratify;
           if(!ngl::nlua_table::table_pop(L, "mroleid", lmroleid, "mratify", lmratify))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_SIGN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_SIGN& adata)
       {
            ngl::nlua_table::table_push(L, "mfamilid", adata.mfamilid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_SIGN& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfamilid;
           if(!ngl::nlua_table::table_pop(L, "mfamilid", lmfamilid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_SIGN& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmstat;
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
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND& adata)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND& adata, bool apop = true)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ADD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ADD& adata)
       {
            ngl::nlua_table::table_push(L, "mfriedid", adata.mfriedid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ADD& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfriedid;
           if(!ngl::nlua_table::table_pop(L, "mfriedid", lmfriedid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ADD& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mfriedid", adata.mfriedid(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfriedid;
           int32_t lmstat;
           if(!ngl::nlua_table::table_pop(L, "mfriedid", lmfriedid, "mstat", lmstat))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ERASE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ERASE& adata)
       {
            ngl::nlua_table::table_push(L, "mfriedid", adata.mfriedid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ERASE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfriedid;
           if(!ngl::nlua_table::table_pop(L, "mfriedid", lmfriedid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ERASE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mfriedid", adata.mfriedid(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfriedid;
           int32_t lmstat;
           if(!ngl::nlua_table::table_pop(L, "mfriedid", lmfriedid, "mstat", lmstat))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata)
       {
            ngl::nlua_table::table_push(L, "mfriedid", adata.mfriedid(),"mratify", adata.mratify());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfriedid;
           bool lmratify;
           if(!ngl::nlua_table::table_pop(L, "mfriedid", lmfriedid, "mratify", lmratify))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mfriedid", adata.mfriedid(),"mratify", adata.mratify(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmfriedid;
           bool lmratify;
           int32_t lmstat;
           if(!ngl::nlua_table::table_pop(L, "mfriedid", lmfriedid, "mratify", lmratify, "mstat", lmstat))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mfriends", adata.mfriends(),"mapplyfriends", adata.mapplyfriends());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mfriends",  *adata.mutable_mfriends(), "mapplyfriends",  *adata.mutable_mapplyfriends()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_GET_TIME>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_GET_TIME& adata)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_GET_TIME& adata, bool apop = true)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_GET_TIME& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_GET_TIME& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mutc", adata.mutc());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmutc;
           if(!ngl::nlua_table::table_pop(L, "mutc", lmutc))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_HEARTBEAT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_HEARTBEAT& adata)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_HEARTBEAT& adata, bool apop = true)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_HEARTBEAT& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_HEARTBEAT& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_KCPSESSION>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_KCPSESSION& adata)
       {
            ngl::nlua_table::table_push(L, "mserverid", adata.mserverid(),"muip", adata.muip(),"muport", adata.muport(),"mconv", adata.mconv());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_KCPSESSION& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmserverid;
           std::string lmuip;
           int32_t lmuport;
           int32_t lmconv;
           if(!ngl::nlua_table::table_pop(L, "mserverid", lmserverid, "muip", lmuip, "muport", lmuport, "mconv", lmconv))
           {
               return false;
           }
           adata.set_mserverid(lmserverid);
           adata.set_muip(lmuip);
           adata.set_muport(lmuport);
           adata.set_mconv(lmconv);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_KCPSESSION& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_KCPSESSION& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mkcpsession", adata.mkcpsession());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           std::string lmkcpsession;
           if(!ngl::nlua_table::table_pop(L, "mkcpsession", lmkcpsession))
           {
               return false;
           }
           adata.set_mkcpsession(lmkcpsession);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DEL>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DEL& adata)
       {
            ngl::nlua_table::table_push(L, "mmailid", adata.mmailid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DEL& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmmailid;
           if(!ngl::nlua_table::table_pop(L, "mmailid", lmmailid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DEL& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mmailid", adata.mmailid(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmmailid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, "mmailid", lmmailid, "mstat", lmstat))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DRAW>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DRAW& adata)
       {
            ngl::nlua_table::table_push(L, "mmailid", adata.mmailid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DRAW& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmmailid;
           if(!ngl::nlua_table::table_pop(L, "mmailid", lmmailid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DRAW& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mmailid", adata.mmailid(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmmailid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, "mmailid", lmmailid, "mstat", lmstat))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_LIST>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_LIST& adata)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_LIST& adata, bool apop = true)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_LIST& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_LIST& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mmail", adata.mmail());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mmail",  *adata.mutable_mmail()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_READ>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_READ& adata)
       {
            ngl::nlua_table::table_push(L, "mmailid", adata.mmailid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_READ& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmmailid;
           if(!ngl::nlua_table::table_pop(L, "mmailid", lmmailid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_READ& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mmailid", adata.mmailid(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmmailid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, "mmailid", lmmailid, "mstat", lmstat))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_MSG_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MSG_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mmsg", adata.mmsg());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MSG_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           std::string lmmsg;
           if(!ngl::nlua_table::table_pop(L, "mmsg", lmmsg))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MSG_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_NOTICE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_NOTICE& adata)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_NOTICE& adata, bool apop = true)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_NOTICE& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_NOTICE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_NOTICE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mnotices", adata.mnotices());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mnotices",  *adata.mutable_mnotices()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_RANKLIST>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RANKLIST& adata)
       {
            ngl::nlua_table::table_push(L, "miscross", adata.miscross(),"mtype", adata.mtype(),"mactivityid", adata.mactivityid(),"mpage", adata.mpage());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RANKLIST& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           bool lmiscross;
           pbdb::eranklist lmtype;
           int32_t lmactivityid;
           int32_t lmpage;
           if(!ngl::nlua_table::table_pop(L, "miscross", lmiscross, "mtype", lmtype, "mactivityid", lmactivityid, "mpage", lmpage))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RANKLIST& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mtype", adata.mtype(),"mitems", adata.mitems(),"mpage", adata.mpage(),"mcount", adata.mcount(),"mrolerank", adata.mrolerank());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbdb::eranklist lmtype;
           int32_t lmpage;
           int32_t lmcount;
           int32_t lmrolerank;
           if(!ngl::nlua_table::table_pop(L, "mtype", lmtype, "mitems",  *adata.mutable_mitems(), "mpage", lmpage, "mcount", lmcount, "mrolerank", lmrolerank))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_RECHARGE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RECHARGE& adata)
       {
            ngl::nlua_table::table_push(L, "mrechargeid", adata.mrechargeid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RECHARGE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmrechargeid;
           if(!ngl::nlua_table::table_pop(L, "mrechargeid", lmrechargeid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RECHARGE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mrechargeid", adata.mrechargeid(),"mstat", adata.mstat(),"morderid", adata.morderid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmrechargeid;
           pbnet::PROBUFF_NET_RECHARGE_RESPONSE::Estat lmstat;
           std::string lmorderid;
           if(!ngl::nlua_table::table_pop(L, "mrechargeid", lmrechargeid, "mstat", lmstat, "morderid", lmorderid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mitems", adata.mitems());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mitems",  *adata.mutable_mitems()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_ROLESTAT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLESTAT& adata)
       {
            ngl::nlua_table::table_push(L, "mstat", adata.mstat(),"mlogicstat", adata.mlogicstat(),"mroleid", adata.mroleid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLESTAT& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbnet::PROBUFF_NET_ROLESTAT::stat lmstat;
           pbnet::PROBUFF_NET_ROLESTAT::logic_stat lmlogicstat;
           int64_t lmroleid;
           if(!ngl::nlua_table::table_pop(L, "mstat", lmstat, "mlogicstat", lmlogicstat, "mroleid", lmroleid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLESTAT& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_LOGIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
       {
            ngl::nlua_table::table_push(L, "mroleid", adata.mroleid(),"msession", adata.msession(),"miscreate", adata.miscreate(),"marea", adata.marea(),"mgatewayid", adata.mgatewayid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_LOGIN& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmroleid;
           std::string lmsession;
           bool lmiscreate;
           int32_t lmarea;
           int32_t lmgatewayid;
           if(!ngl::nlua_table::table_pop(L, "mroleid", lmroleid, "msession", lmsession, "miscreate", lmiscreate, "marea", lmarea, "mgatewayid", lmgatewayid))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           adata.set_msession(lmsession);
           adata.set_miscreate(lmiscreate);
           adata.set_marea(lmarea);
           adata.set_mgatewayid(lmgatewayid);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_SYNC>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_SYNC& adata)
       {
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_SYNC& adata, bool apop = true)
       {
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_SYNC& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_SYNC& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mrole", adata.mrole(),"mbrief", adata.mbrief(),"mbag", adata.mbag(),"mtask", adata.mtask());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mrole",  *adata.mutable_mrole(), "mbrief",  *adata.mutable_mbrief(), "mbag",  *adata.mutable_mbag(), "mtask",  *adata.mutable_mtask()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_SWITCH_LINE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SWITCH_LINE& adata)
       {
            ngl::nlua_table::table_push(L, "mline", adata.mline());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SWITCH_LINE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmline;
           if(!ngl::nlua_table::table_pop(L, "mline", lmline))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SWITCH_LINE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mline", adata.mline(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmline;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, "mline", lmline, "mstat", lmstat))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata)
       {
            ngl::nlua_table::table_push(L, "mmodules", adata.mmodules());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mmodules",  *adata.mutable_mmodules()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_POSITION>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_POSITION& adata)
       {
            ngl::nlua_table::table_push(L, "mposition", adata.mposition());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_POSITION& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "mposition",  *adata.mutable_mposition()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_POSITION& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_UNIT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_UNIT& adata)
       {
            ngl::nlua_table::table_push(L, "munits", adata.munits());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_UNIT& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           if(!ngl::nlua_table::table_pop(L, "munits",  *adata.mutable_munits()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_UNIT& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata)
       {
            ngl::nlua_table::table_push(L, "mtaskid", adata.mtaskid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmtaskid;
           if(!ngl::nlua_table::table_pop(L, "mtaskid", lmtaskid))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata)
       {
            ngl::nlua_table::table_push(L, "mtaskid", adata.mtaskid(),"mstat", adata.mstat(),"mdrop", adata.mdrop());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmtaskid;
           bool lmstat;
           if(!ngl::nlua_table::table_pop(L, "mtaskid", lmtaskid, "mstat", lmstat, "mdrop",  *adata.mutable_mdrop()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata)
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
   struct serialize_lua<pbnet::PROBUFF_NET_TESTLUA>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TESTLUA& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mvalue", adata.mvalue(),"mdata", adata.mdata());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TESTLUA& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           std::string lmvalue;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mvalue", lmvalue, "mdata",  *adata.mutable_mdata()))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TESTLUA& adata)
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
   struct serialize_lua<pbnet::UNIT>
   {
       static void stack_push(lua_State* L, const pbnet::UNIT& adata)
       {
            ngl::nlua_table::table_push(L, "mtype", adata.mtype(),"mid", adata.mid(),"mposition", adata.mposition(),"mmodules", adata.mmodules(),"mstat", adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::UNIT& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           pbnet::eunit lmtype;
           int64_t lmid;
           pbnet::eunitstat lmstat;
           if(!ngl::nlua_table::table_pop(L, "mtype", lmtype, "mid", lmid, "mposition",  *adata.mutable_mposition(), "mmodules",  *adata.mutable_mmodules(), "mstat", lmstat))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mid(lmid);
           adata.set_mstat(lmstat);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::UNIT& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::UNIT& adata)
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
   struct serialize_lua<pbnet::UNIT_POSITION>
   {
       static void stack_push(lua_State* L, const pbnet::UNIT_POSITION& adata)
       {
            ngl::nlua_table::table_push(L, "mid", adata.mid(),"mposition", adata.mposition(),"mangle", adata.mangle(),"mspeed", adata.mspeed());
       }
       static bool stack_pop(lua_State* L, pbnet::UNIT_POSITION& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmid;
           int32_t lmangle;
           int32_t lmspeed;
           if(!ngl::nlua_table::table_pop(L, "mid", lmid, "mposition",  *adata.mutable_mposition(), "mangle", lmangle, "mspeed", lmspeed))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mangle(lmangle);
           adata.set_mspeed(lmspeed);
           if (apop)
           {
               lua_pop(L, 1);
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::UNIT_POSITION& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::UNIT_POSITION& adata)
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
   struct serialize_lua<pbnet::UnitAttribute>
   {
       static void stack_push(lua_State* L, const pbnet::UnitAttribute& adata)
       {
            ngl::nlua_table::table_push(L, "mtype", adata.mtype(),"mvalue", adata.mvalue());
       }
       static bool stack_pop(lua_State* L, pbnet::UnitAttribute& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmtype;
           int64_t lmvalue;
           if(!ngl::nlua_table::table_pop(L, "mtype", lmtype, "mvalue", lmvalue))
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
       static void table_push(lua_State * L, const char* aname, const pbnet::UnitAttribute& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::UnitAttribute& adata)
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
   struct serialize_lua<pbnet::UnitModule>
   {
       static void stack_push(lua_State* L, const pbnet::UnitModule& adata)
       {
            ngl::nlua_table::table_push(L, "mmtype", adata.mmtype(),"mmodules", adata.mmodules());
       }
       static bool stack_pop(lua_State* L, pbnet::UnitModule& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmmtype;
           if(!ngl::nlua_table::table_pop(L, "mmtype", lmmtype, "mmodules",  *adata.mutable_mmodules()))
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
       static void table_push(lua_State * L, const char* aname, const pbnet::UnitModule& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::UnitModule& adata)
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
   struct serialize_lua<pbnet::VECTOR2>
   {
       static void stack_push(lua_State* L, const pbnet::VECTOR2& adata)
       {
            ngl::nlua_table::table_push(L, "mx", adata.mx(),"my", adata.my());
       }
       static bool stack_pop(lua_State* L, pbnet::VECTOR2& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmx;
           int32_t lmy;
           if(!ngl::nlua_table::table_pop(L, "mx", lmx, "my", lmy))
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
       static void table_push(lua_State * L, const char* aname, const pbnet::VECTOR2& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::VECTOR2& adata)
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
   struct serialize_lua<pbnet::VECTOR3>
   {
       static void stack_push(lua_State* L, const pbnet::VECTOR3& adata)
       {
            ngl::nlua_table::table_push(L, "mx", adata.mx(),"my", adata.my(),"mz", adata.mz());
       }
       static bool stack_pop(lua_State* L, pbnet::VECTOR3& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmx;
           int32_t lmy;
           int32_t lmz;
           if(!ngl::nlua_table::table_pop(L, "mx", lmx, "my", lmy, "mz", lmz))
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
       static void table_push(lua_State * L, const char* aname, const pbnet::VECTOR3& adata)
       {
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::VECTOR3& adata)
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
   struct serialize_lua<pbnet::chatitem>
   {
       static void stack_push(lua_State* L, const pbnet::chatitem& adata)
       {
            ngl::nlua_table::table_push(L, "mroleid", adata.mroleid(),"mrolename", adata.mrolename(),"mcontent", adata.mcontent(),"mutc", adata.mutc());
       }
       static bool stack_pop(lua_State* L, pbnet::chatitem& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int64_t lmroleid;
           std::string lmrolename;
           std::string lmcontent;
           int32_t lmutc;
           if(!ngl::nlua_table::table_pop(L, "mroleid", lmroleid, "mrolename", lmrolename, "mcontent", lmcontent, "mutc", lmutc))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::chatitem& adata)
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
   struct serialize_lua<pbnet::drop_item>
   {
       static void stack_push(lua_State* L, const pbnet::drop_item& adata)
       {
            ngl::nlua_table::table_push(L, "mitem", adata.mitem(),"mcount", adata.mcount());
       }
       static bool stack_pop(lua_State* L, pbnet::drop_item& adata, bool apop = true)
       {
           if (ngl::nlua_table::table_isnil(L))
           {
               return true;
           }
           int32_t lmitem;
           int32_t lmcount;
           if(!ngl::nlua_table::table_pop(L, "mitem", lmitem, "mcount", lmcount))
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
           ngl::nlua_table::table_start_push(L, aname);
           stack_push(L, adata);
           ngl::nlua_table::table_finish_push(L, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::drop_item& adata)
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
