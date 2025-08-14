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
   struct serialize_lua<pbnet::PROBUFF_NET_ACOUNT_LOGIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.marea(),adata.maccount(),adata.mpassword());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata, bool apop = true)
       {
           int32_t lmarea;
           std::string lmaccount;
           std::string lmpassword;
           if(!nlua_stack::stack_pop(L, lmarea, lmaccount, lmpassword))
           {
               return false;
           }
           adata.set_marea(lmarea);
           adata.set_maccount(lmaccount);
           adata.set_mpassword(lmpassword);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ACOUNT_LOGIN& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.marea(),adata.mroleid(),adata.msession(),adata.maccount(),adata.mgatewayid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata, bool apop = true)
       {
           int32_t lmarea;
           int64_t lmroleid;
           std::string lmsession;
           std::string lmaccount;
           int32_t lmgatewayid;
           if(!nlua_stack::stack_pop(L, lmarea, lmroleid, lmsession, lmaccount, lmgatewayid))
           {
               return false;
           }
           adata.set_marea(lmarea);
           adata.set_mroleid(lmroleid);
           adata.set_msession(lmsession);
           adata.set_maccount(lmaccount);
           adata.set_mgatewayid(lmgatewayid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_SYNC& adata)
       {
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_SYNC& adata)
       {
           return true;
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mbag());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mbag()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mdelitems(),adata.mdelnostackitems(),adata.madditems(),adata.maddnostackitems());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mdelitems(),  *adata.mutable_mdelnostackitems(),  *adata.mutable_madditems(),  *adata.mutable_maddnostackitems()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_UPDATE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE::additem>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mcount());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata, bool apop = true)
       {
           int32_t lmid;
           int32_t lmcount;
           if(!nlua_stack::stack_pop(L, lmid, lmcount))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mcount(lmcount);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE::additem& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_BAG_UPDATE::delitem>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mcount());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata, bool apop = true)
       {
           int32_t lmid;
           int32_t lmcount;
           if(!nlua_stack::stack_pop(L, lmid, lmcount))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mcount(lmcount);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_BAG_UPDATE::delitem& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_CHANGE_ANGLE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHANGE_ANGLE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mposition());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHANGE_ANGLE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mposition()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_CHANGE_ANGLE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHANGE_ANGLE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_CHAT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHAT& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtype(),adata.mchannelid(),adata.mcontent());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHAT& adata, bool apop = true)
       {
           int32_t lmtype;
           int32_t lmchannelid;
           std::string lmcontent;
           if(!nlua_stack::stack_pop(L, lmtype, lmchannelid, lmcontent))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mchannelid(lmchannelid);
           adata.set_mcontent(lmcontent);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_CHAT& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHAT& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_CHAT_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CHAT_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtype(),adata.mchannelid(),adata.mchatlist(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CHAT_RESPONSE& adata, bool apop = true)
       {
           int32_t lmtype;
           int32_t lmchannelid;
           bool lmstat;
           if(!nlua_stack::stack_pop(L, lmtype, lmchannelid,  *adata.mutable_mchatlist(), lmstat))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mchannelid(lmchannelid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_CHAT_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CHAT_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_CMD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_CMD& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mcmd());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_CMD& adata, bool apop = true)
       {
           std::string lmcmd;
           if(!nlua_stack::stack_pop(L, lmcmd))
           {
               return false;
           }
           adata.set_mcmd(lmcmd);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_CMD& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_CMD& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mrechargeid(),adata.morderid(),adata.mgold(),adata.mitems());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata, bool apop = true)
       {
           int32_t lmrechargeid;
           std::string lmorderid;
           int32_t lmgold;
           if(!nlua_stack::stack_pop(L, lmrechargeid, lmorderid, lmgold,  *adata.mutable_mitems()))
           {
               return false;
           }
           adata.set_mrechargeid(lmrechargeid);
           adata.set_morderid(lmorderid);
           adata.set_mgold(lmgold);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.misenter(),adata.munits());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata, bool apop = true)
       {
           bool lmisenter;
           if(!nlua_stack::stack_pop(L, lmisenter,  *adata.mutable_munits()))
           {
               return false;
           }
           adata.set_misenter(lmisenter);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ERROR>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ERROR& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.merrmessage());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ERROR& adata, bool apop = true)
       {
           std::string lmerrmessage;
           if(!nlua_stack::stack_pop(L, lmerrmessage))
           {
               return false;
           }
           adata.set_merrmessage(lmerrmessage);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ERROR& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ERROR& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CEDE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CEDE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mroleid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CEDE& adata, bool apop = true)
       {
           int64_t lmroleid;
           if(!nlua_stack::stack_pop(L, lmroleid))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CEDE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CEDE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata, bool apop = true)
       {
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfamilid(),adata.mname());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata, bool apop = true)
       {
           int64_t lmfamilid;
           std::string lmname;
           if(!nlua_stack::stack_pop(L, lmfamilid, lmname))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           adata.set_mname(lmname);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CHANGENAME& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata, bool apop = true)
       {
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CREATE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mname());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CREATE& adata, bool apop = true)
       {
           std::string lmname;
           if(!nlua_stack::stack_pop(L, lmname))
           {
               return false;
           }
           adata.set_mname(lmname);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata, bool apop = true)
       {
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_INFO& adata)
       {
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_INFO& adata)
       {
           return true;
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat(),adata.minfo(),adata.mmember());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata, bool apop = true)
       {
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmstat,  *adata.mutable_minfo(),  *adata.mutable_mmember()))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_JOIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_JOIN& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfamilid(),adata.mapply());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_JOIN& adata, bool apop = true)
       {
           int64_t lmfamilid;
           bool lmapply;
           if(!nlua_stack::stack_pop(L, lmfamilid, lmapply))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           adata.set_mapply(lmapply);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_JOIN& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_JOIN& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata, bool apop = true)
       {
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LEAVE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LEAVE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfamilid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LEAVE& adata, bool apop = true)
       {
           int64_t lmfamilid;
           if(!nlua_stack::stack_pop(L, lmfamilid))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LEAVE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LEAVE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata, bool apop = true)
       {
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LIST>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LIST& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfamilid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LIST& adata, bool apop = true)
       {
           int64_t lmfamilid;
           if(!nlua_stack::stack_pop(L, lmfamilid))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LIST& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LIST& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfamilid(),adata.mfamily());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata, bool apop = true)
       {
           int64_t lmfamilid;
           if(!nlua_stack::stack_pop(L, lmfamilid,  *adata.mutable_mfamily()))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mroleid(),adata.mratify());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata, bool apop = true)
       {
           int64_t lmroleid;
           bool lmratify;
           if(!nlua_stack::stack_pop(L, lmroleid, lmratify))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           adata.set_mratify(lmratify);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata, bool apop = true)
       {
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_SIGN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_SIGN& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfamilid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_SIGN& adata, bool apop = true)
       {
           int64_t lmfamilid;
           if(!nlua_stack::stack_pop(L, lmfamilid))
           {
               return false;
           }
           adata.set_mfamilid(lmfamilid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_SIGN& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_SIGN& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata, bool apop = true)
       {
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmstat))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND& adata)
       {
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND& adata)
       {
           return true;
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ADD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ADD& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfriedid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ADD& adata, bool apop = true)
       {
           int64_t lmfriedid;
           if(!nlua_stack::stack_pop(L, lmfriedid))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_ADD& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ADD& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfriedid(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata, bool apop = true)
       {
           int64_t lmfriedid;
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmfriedid, lmstat))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ERASE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ERASE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfriedid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ERASE& adata, bool apop = true)
       {
           int64_t lmfriedid;
           if(!nlua_stack::stack_pop(L, lmfriedid))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_ERASE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ERASE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfriedid(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata, bool apop = true)
       {
           int64_t lmfriedid;
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmfriedid, lmstat))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfriedid(),adata.mratify());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata, bool apop = true)
       {
           int64_t lmfriedid;
           bool lmratify;
           if(!nlua_stack::stack_pop(L, lmfriedid, lmratify))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           adata.set_mratify(lmratify);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfriedid(),adata.mratify(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata, bool apop = true)
       {
           int64_t lmfriedid;
           bool lmratify;
           int32_t lmstat;
           if(!nlua_stack::stack_pop(L, lmfriedid, lmratify, lmstat))
           {
               return false;
           }
           adata.set_mfriedid(lmfriedid);
           adata.set_mratify(lmratify);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_FRIEND_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mfriends(),adata.mapplyfriends());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mfriends(),  *adata.mutable_mapplyfriends()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_FRIEND_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_GET_TIME& adata)
       {
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_GET_TIME& adata)
       {
           return true;
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mutc());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata, bool apop = true)
       {
           int32_t lmutc;
           if(!nlua_stack::stack_pop(L, lmutc))
           {
               return false;
           }
           adata.set_mutc(lmutc);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_GET_TIME_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_HEARTBEAT& adata)
       {
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_HEARTBEAT& adata)
       {
           return true;
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_KCPSESSION>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_KCPSESSION& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mserverid(),adata.muip(),adata.muport(),adata.mconv());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_KCPSESSION& adata, bool apop = true)
       {
           int64_t lmserverid;
           std::string lmuip;
           int32_t lmuport;
           int32_t lmconv;
           if(!nlua_stack::stack_pop(L, lmserverid, lmuip, lmuport, lmconv))
           {
               return false;
           }
           adata.set_mserverid(lmserverid);
           adata.set_muip(lmuip);
           adata.set_muport(lmuport);
           adata.set_mconv(lmconv);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_KCPSESSION& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_KCPSESSION& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mkcpsession());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata, bool apop = true)
       {
           std::string lmkcpsession;
           if(!nlua_stack::stack_pop(L, lmkcpsession))
           {
               return false;
           }
           adata.set_mkcpsession(lmkcpsession);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DEL>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DEL& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmailid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DEL& adata, bool apop = true)
       {
           int64_t lmmailid;
           if(!nlua_stack::stack_pop(L, lmmailid))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_DEL& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DEL& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmailid(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata, bool apop = true)
       {
           int64_t lmmailid;
           bool lmstat;
           if(!nlua_stack::stack_pop(L, lmmailid, lmstat))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DRAW>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DRAW& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmailid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DRAW& adata, bool apop = true)
       {
           int64_t lmmailid;
           if(!nlua_stack::stack_pop(L, lmmailid))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_DRAW& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DRAW& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmailid(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata, bool apop = true)
       {
           int64_t lmmailid;
           bool lmstat;
           if(!nlua_stack::stack_pop(L, lmmailid, lmstat))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_LIST& adata)
       {
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_LIST& adata)
       {
           return true;
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmail());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mmail()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_READ>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_READ& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmailid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_READ& adata, bool apop = true)
       {
           int64_t lmmailid;
           if(!nlua_stack::stack_pop(L, lmmailid))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_READ& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_READ& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmailid(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata, bool apop = true)
       {
           int64_t lmmailid;
           bool lmstat;
           if(!nlua_stack::stack_pop(L, lmmailid, lmstat))
           {
               return false;
           }
           adata.set_mmailid(lmmailid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_MSG_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_MSG_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmsg());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_MSG_RESPONSE& adata, bool apop = true)
       {
           std::string lmmsg;
           if(!nlua_stack::stack_pop(L, lmmsg))
           {
               return false;
           }
           adata.set_mmsg(lmmsg);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_MSG_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_MSG_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_NOTICE& adata)
       {
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_NOTICE& adata)
       {
           return true;
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_NOTICE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mnotices());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mnotices()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_NOTICE_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_RANKLIST>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RANKLIST& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.miscross(),adata.mtype(),adata.mactivityid(),adata.mpage());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RANKLIST& adata, bool apop = true)
       {
           bool lmiscross;
           pbdb::eranklist lmtype;
           int32_t lmactivityid;
           int32_t lmpage;
           if(!nlua_stack::stack_pop(L, lmiscross, lmtype, lmactivityid, lmpage))
           {
               return false;
           }
           adata.set_miscross(lmiscross);
           adata.set_mtype(lmtype);
           adata.set_mactivityid(lmactivityid);
           adata.set_mpage(lmpage);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_RANKLIST& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RANKLIST& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtype(),adata.mitems(),adata.mpage(),adata.mcount(),adata.mrolerank());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata, bool apop = true)
       {
           pbdb::eranklist lmtype;
           int32_t lmpage;
           int32_t lmcount;
           int32_t lmrolerank;
           if(!nlua_stack::stack_pop(L, lmtype,  *adata.mutable_mitems(), lmpage, lmcount, lmrolerank))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mpage(lmpage);
           adata.set_mcount(lmcount);
           adata.set_mrolerank(lmrolerank);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RANKLIST_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_RECHARGE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RECHARGE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mrechargeid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RECHARGE& adata, bool apop = true)
       {
           int32_t lmrechargeid;
           if(!nlua_stack::stack_pop(L, lmrechargeid))
           {
               return false;
           }
           adata.set_mrechargeid(lmrechargeid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_RECHARGE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RECHARGE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mrechargeid(),adata.mstat(),adata.morderid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata, bool apop = true)
       {
           int32_t lmrechargeid;
           pbnet::PROBUFF_NET_RECHARGE_RESPONSE::Estat lmstat;
           std::string lmorderid;
           if(!nlua_stack::stack_pop(L, lmrechargeid, lmstat, lmorderid))
           {
               return false;
           }
           adata.set_mrechargeid(lmrechargeid);
           adata.set_mstat(lmstat);
           adata.set_morderid(lmorderid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_RECHARGE_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mitems());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mitems()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLESTAT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLESTAT& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mstat(),adata.mlogicstat(),adata.mroleid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLESTAT& adata, bool apop = true)
       {
           pbnet::PROBUFF_NET_ROLESTAT::stat lmstat;
           pbnet::PROBUFF_NET_ROLESTAT::logic_stat lmlogicstat;
           int64_t lmroleid;
           if(!nlua_stack::stack_pop(L, lmstat, lmlogicstat, lmroleid))
           {
               return false;
           }
           adata.set_mstat(lmstat);
           adata.set_mlogicstat(lmlogicstat);
           adata.set_mroleid(lmroleid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLESTAT& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLESTAT& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_LOGIN>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mroleid(),adata.msession(),adata.miscreate(),adata.marea(),adata.mgatewayid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_LOGIN& adata, bool apop = true)
       {
           int64_t lmroleid;
           std::string lmsession;
           bool lmiscreate;
           int32_t lmarea;
           int32_t lmgatewayid;
           if(!nlua_stack::stack_pop(L, lmroleid, lmsession, lmiscreate, lmarea, lmgatewayid))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           adata.set_msession(lmsession);
           adata.set_miscreate(lmiscreate);
           adata.set_marea(lmarea);
           adata.set_mgatewayid(lmgatewayid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
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
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_SYNC& adata)
       {
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_SYNC& adata)
       {
           return true;
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mrole(),adata.mbrief(),adata.mbag(),adata.mtask());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mrole(),  *adata.mutable_mbrief(),  *adata.mutable_mbag(),  *adata.mutable_mtask()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SWITCH_LINE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SWITCH_LINE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mline());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SWITCH_LINE& adata, bool apop = true)
       {
           int32_t lmline;
           if(!nlua_stack::stack_pop(L, lmline))
           {
               return false;
           }
           adata.set_mline(lmline);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SWITCH_LINE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SWITCH_LINE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mline(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata, bool apop = true)
       {
           int32_t lmline;
           bool lmstat;
           if(!nlua_stack::stack_pop(L, lmline, lmstat))
           {
               return false;
           }
           adata.set_mline(lmline);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmodules());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mmodules()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_ATTRIBUTE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_POSITION>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_POSITION& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mposition());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_POSITION& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_mposition()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SYNC_POSITION& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_POSITION& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_SYNC_UNIT>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_SYNC_UNIT& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.munits());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_SYNC_UNIT& adata, bool apop = true)
       {
           if(!nlua_stack::stack_pop(L,  *adata.mutable_munits()))
           {
               return false;
           }
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_SYNC_UNIT& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_SYNC_UNIT& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtaskid());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata, bool apop = true)
       {
           int32_t lmtaskid;
           if(!nlua_stack::stack_pop(L, lmtaskid))
           {
               return false;
           }
           adata.set_mtaskid(lmtaskid);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtaskid(),adata.mstat(),adata.mdrop());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata, bool apop = true)
       {
           int32_t lmtaskid;
           bool lmstat;
           if(!nlua_stack::stack_pop(L, lmtaskid, lmstat,  *adata.mutable_mdrop()))
           {
               return false;
           }
           adata.set_mtaskid(lmtaskid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::PROBUFF_NET_TESTLUA>
   {
       static void stack_push(lua_State* L, const pbnet::PROBUFF_NET_TESTLUA& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mvalue(),adata.mdata());
       }
       static bool stack_pop(lua_State* L, pbnet::PROBUFF_NET_TESTLUA& adata, bool apop = true)
       {
           int64_t lmid;
           std::string lmvalue;
           if(!nlua_stack::stack_pop(L, lmid, lmvalue,  *adata.mutable_mdata()))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mvalue(lmvalue);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::PROBUFF_NET_TESTLUA& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::PROBUFF_NET_TESTLUA& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::UNIT>
   {
       static void stack_push(lua_State* L, const pbnet::UNIT& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtype(),adata.mid(),adata.mposition(),adata.mmodules(),adata.mstat());
       }
       static bool stack_pop(lua_State* L, pbnet::UNIT& adata, bool apop = true)
       {
           pbnet::eunit lmtype;
           int64_t lmid;
           pbnet::eunitstat lmstat;
           if(!nlua_stack::stack_pop(L, lmtype, lmid,  *adata.mutable_mposition(),  *adata.mutable_mmodules(), lmstat))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mid(lmid);
           adata.set_mstat(lmstat);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::UNIT& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::UNIT& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::UNIT_POSITION>
   {
       static void stack_push(lua_State* L, const pbnet::UNIT_POSITION& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mid(),adata.mposition(),adata.mangle(),adata.mspeed());
       }
       static bool stack_pop(lua_State* L, pbnet::UNIT_POSITION& adata, bool apop = true)
       {
           int64_t lmid;
           int32_t lmangle;
           int32_t lmspeed;
           if(!nlua_stack::stack_pop(L, lmid,  *adata.mutable_mposition(), lmangle, lmspeed))
           {
               return false;
           }
           adata.set_mid(lmid);
           adata.set_mangle(lmangle);
           adata.set_mspeed(lmspeed);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::UNIT_POSITION& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::UNIT_POSITION& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::UnitAttribute>
   {
       static void stack_push(lua_State* L, const pbnet::UnitAttribute& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mtype(),adata.mvalue());
       }
       static bool stack_pop(lua_State* L, pbnet::UnitAttribute& adata, bool apop = true)
       {
           int32_t lmtype;
           int64_t lmvalue;
           if(!nlua_stack::stack_pop(L, lmtype, lmvalue))
           {
               return false;
           }
           adata.set_mtype(lmtype);
           adata.set_mvalue(lmvalue);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::UnitAttribute& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::UnitAttribute& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::UnitModule>
   {
       static void stack_push(lua_State* L, const pbnet::UnitModule& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mmtype(),adata.mmodules());
       }
       static bool stack_pop(lua_State* L, pbnet::UnitModule& adata, bool apop = true)
       {
           int32_t lmmtype;
           if(!nlua_stack::stack_pop(L, lmmtype,  *adata.mutable_mmodules()))
           {
               return false;
           }
           adata.set_mmtype(lmmtype);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::UnitModule& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::UnitModule& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::VECTOR2>
   {
       static void stack_push(lua_State* L, const pbnet::VECTOR2& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mx(),adata.my());
       }
       static bool stack_pop(lua_State* L, pbnet::VECTOR2& adata, bool apop = true)
       {
           int32_t lmx;
           int32_t lmy;
           if(!nlua_stack::stack_pop(L, lmx, lmy))
           {
               return false;
           }
           adata.set_mx(lmx);
           adata.set_my(lmy);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::VECTOR2& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::VECTOR2& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::VECTOR3>
   {
       static void stack_push(lua_State* L, const pbnet::VECTOR3& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mx(),adata.my(),adata.mz());
       }
       static bool stack_pop(lua_State* L, pbnet::VECTOR3& adata, bool apop = true)
       {
           int32_t lmx;
           int32_t lmy;
           int32_t lmz;
           if(!nlua_stack::stack_pop(L, lmx, lmy, lmz))
           {
               return false;
           }
           adata.set_mx(lmx);
           adata.set_my(lmy);
           adata.set_mz(lmz);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::VECTOR3& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::VECTOR3& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::chatitem>
   {
       static void stack_push(lua_State* L, const pbnet::chatitem& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mroleid(),adata.mrolename(),adata.mcontent(),adata.mutc());
       }
       static bool stack_pop(lua_State* L, pbnet::chatitem& adata, bool apop = true)
       {
           int64_t lmroleid;
           std::string lmrolename;
           std::string lmcontent;
           int32_t lmutc;
           if(!nlua_stack::stack_pop(L, lmroleid, lmrolename, lmcontent, lmutc))
           {
               return false;
           }
           adata.set_mroleid(lmroleid);
           adata.set_mrolename(lmrolename);
           adata.set_mcontent(lmcontent);
           adata.set_mutc(lmutc);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::chatitem& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::chatitem& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
   template <>
   struct serialize_lua<pbnet::drop_item>
   {
       static void stack_push(lua_State* L, const pbnet::drop_item& adata)
       {
            lua_newtable(L);
            nlua_stack::stack_push(L, adata.mitem(),adata.mcount());
       }
       static bool stack_pop(lua_State* L, pbnet::drop_item& adata, bool apop = true)
       {
           int32_t lmitem;
           int32_t lmcount;
           if(!nlua_stack::stack_pop(L, lmitem, lmcount))
           {
               return false;
           }
           adata.set_mitem(lmitem);
           adata.set_mcount(lmcount);
           return true;
       }
       static void table_push(lua_State * L, const char* aname, const pbnet::drop_item& adata)
       {
           stack_push(L, adata);
           lua_setfield(L, -2, aname);
       }
       static bool table_pop(lua_State * L, const char* aname, pbnet::drop_item& adata)
       {
           lua_getfield(L, -1, aname);
           return stack_pop(L, adata);
       }
   };
}//namespace ngl
