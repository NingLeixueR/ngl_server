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
#pragma once

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <vector>
#include <unordered_set>

#include "tools.h"
#include "type.h"

namespace ngl
{
    enum epb_field
    {
        epb_field_read,		    // 读
        epb_field_write,	    // 写(既然可写必定也可读)
    };

    // 工具类：只复制指定字段编号的内容
    class pb_field
    {
    public:
        // 拷贝 src 到 dst，但只复制字段编号在 field_numbers 中的字段
        // field_numbers：需要复制的字段编号列表（如 {1, 3, 5}）
        static void copy(const google::protobuf::Message& src, google::protobuf::Message* dst, const std::map<i32_fieldnumber, epb_field>& field_numbers);
    private:
        // 复制单个字段（处理所有类型的字段：基础类型、repeated、消息类型等）
        static void copyfield(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const google::protobuf::Reflection* src_refl
            , const google::protobuf::Reflection* dst_refl
            , const google::protobuf::FieldDescriptor* field
        );

        // 复制单个非 repeated 字段
        static void copy_single_field(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const google::protobuf::Reflection* src_refl
            , const google::protobuf::Reflection* dst_refl
            , const google::protobuf::FieldDescriptor* field
        );

        // 复制 repeated 字段中的单个元素
        static void copy_repeated_field(const google::protobuf::Message& src,
            google::protobuf::Message* dst,
            const google::protobuf::Reflection* src_refl,
            const google::protobuf::Reflection* dst_refl,
            const google::protobuf::FieldDescriptor* field,
            int index
        );
    public:
        template <typename T>
        static int32_t field_number(const char* afieldname)
        {
            // 1. 获取 db_brief 消息的描述符（Descriptor）
            const google::protobuf::Descriptor* db_brief_descriptor = T::descriptor();
            if (db_brief_descriptor == nullptr) 
            {
                tools::no_core_dump();
                return -1;
            }

            // 2. 根据字段名（"mactivityvalues"）获取该字段的描述符（FieldDescriptor）
            const google::protobuf::FieldDescriptor* field_descriptor = db_brief_descriptor->FindFieldByName(afieldname);
            if (field_descriptor == nullptr) 
            {
                tools::no_core_dump();
                return -1;
            }

            return field_descriptor->number();
        }
    };

}
