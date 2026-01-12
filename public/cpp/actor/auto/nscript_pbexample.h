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
 // 创建时间【2026-01-11 18:30:56】

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
           ngl::nlua_table::table_push(L, aname, { "mplayers", "misconfirm" }, adata.mplayers(), adata.misconfirm());
       }
       static bool stack_pop(lua_State* L, pbexample::PLAYER& adata, bool apop = true, const char* aname = nullptr)
       {
           bool lmisconfirm;
           if (!ngl::nlua_table::table_pop(L, aname, { "mplayers", "misconfirm" }, *adata.mutable_mplayers(), lmisconfirm))
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
  
  
}//namespace ngl
