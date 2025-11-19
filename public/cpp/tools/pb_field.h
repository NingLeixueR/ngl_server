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

#include "nprotocol_template.h"
#include "tools.h"
#include "type.h"

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <unordered_set>
#include <vector>

namespace ngl
{
    // 工具类：只复制指定字段编号的内容
    class pb_field
    {
    public:
        // 拷贝 src 到 dst，但只复制字段编号在 field_numbers 中的字段
        // field_numbers：需要复制的字段编号列表（如 {1, 3, 5}）
        static void copy(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const std::map<i32_fieldnumber, epb_field>& fieldsrc
            , const std::map<i32_fieldnumber, epb_field>& fielddst
        );

        static void copy(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const std::map<i32_fieldnumber, epb_field>& fieldsrc
        );
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
        static std::set<i32_fieldnumber>& field_number(std::set<i32_fieldnumber>& afieldset)
        {
            return afieldset;
        }

        template <typename T>
        static std::set<i32_fieldnumber>& field_number(std::set<i32_fieldnumber>& afieldset, const char* afieldname)
        {
            const google::protobuf::Descriptor* db_brief_descriptor = T::descriptor();
            if (db_brief_descriptor == nullptr) 
            {
                tools::no_core_dump();
                return afieldset;
            }
            const google::protobuf::FieldDescriptor* field_descriptor = db_brief_descriptor->FindFieldByName(afieldname);
            if (field_descriptor == nullptr) 
            {
                tools::no_core_dump();
                return afieldset;
            }
            afieldset.insert(field_descriptor->number());
            return afieldset;
        }

        template <typename T, typename ...TARGS>
        static std::set<i32_fieldnumber>& field_number(std::set<i32_fieldnumber>& afieldset, const char* afieldname, TARGS... args)
        {
            field_number<T>(afieldset, afieldname);
            return field_number<T>(afieldset, args...);
        }

    private:
        template <typename T>
        static void all_field_number(const  std::function<void(i32_fieldnumber, const std::string&)>& afieldfun)
        {
            const google::protobuf::Descriptor* desc = T::descriptor();
            if (desc == nullptr)
            {
                tools::no_core_dump();
                return;
            }

            for (int i = 0; i < desc->field_count(); ++i)
            {
                const google::protobuf::FieldDescriptor* field_desc = desc->field(i);
                if (field_desc == nullptr)
                {
                    continue;
                }
                afieldfun(field_desc->number(), field_desc->name());
            }
        }
    public:
        template <typename T>
        static void all_field_number(std::map<i32_fieldnumber, std::string>& afieldmap)
        {
            all_field_number<T>([&afieldmap](i32_fieldnumber afieldnumber, const std::string& afieldname)
                {
                    afieldmap[afieldnumber] = afieldname;
                });
        }

        template <typename T>
        static void all_field_number(std::set<i32_fieldnumber>& afieldset)
        {
            all_field_number<T>([&afieldset](i32_fieldnumber afieldnumber, const std::string& afieldname)
                {
                    afieldset.insert(afieldnumber);
                });
        }
    };
}
