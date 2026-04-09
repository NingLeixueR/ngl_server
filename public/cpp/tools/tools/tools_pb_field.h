/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for tools.

#pragma once

#include "actor/protocol/nprotocol_template.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <unordered_set>
#include <vector>

namespace ngl::tools
{
    enum
    {
        epb_mid = 1, // proto structure
    };

    // : Specifiedfield content
    class pb_field
    {
    private:
        // Field(handlealltype field: basetype, repeated, messagetype )
        static void copyfield(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const google::protobuf::Reflection* src_refl
            , const google::protobuf::Reflection* dst_refl
            , const google::protobuf::FieldDescriptor* field
        );

        // Copy a single non-repeated field
        static void copy_single_field(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const google::protobuf::Reflection* src_refl
            , const google::protobuf::Reflection* dst_refl
            , const google::protobuf::FieldDescriptor* field
        );

        // Repeated fieldin
        static void copy_repeated_field(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const google::protobuf::Reflection* src_refl
            , const google::protobuf::Reflection* dst_refl
            , const google::protobuf::FieldDescriptor* field
            , int index
        );
    public:
        // Copy src to dst, field field_numbers infield
        // Field_numbers: need to field list( {1, 3, 5})
        static void copy(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const std::map<i32_fieldnumber, epb_field>& fieldsrc
            , const std::map<i32_fieldnumber, epb_field>& fielddst
            , bool amessage /* Whether message, message mid */
        );

        static void copy(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const std::map<i32_fieldnumber, epb_field>& fieldsrc
            , bool amessage /* Whether message, message mid */
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
        static std::set<i32_fieldnumber>& field_number(std::set<i32_fieldnumber>& afieldset, TARGS... args)
        {
            return (field_number<T>(afieldset, args), ...);
        }
    private:
        template <typename T>
        static void field_numbers(const  std::function<void(i32_fieldnumber, const std::string&)>& afieldfun)
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
        static void field_numbers(std::map<i32_fieldnumber, std::string>& afieldmap)
        {
            field_numbers<T>([&afieldmap](i32_fieldnumber afieldnumber, const std::string& afieldname)
                {
                    afieldmap[afieldnumber] = afieldname;
                }
            );
        }

        template <typename T>
        static void field_numbers(std::set<i32_fieldnumber>& afieldset)
        {
	            field_numbers<T>([&afieldset](i32_fieldnumber afieldnumber, [[maybe_unused]] const std::string& afieldname)
	                {
	                    afieldset.insert(afieldnumber);
	                }
            );
        }
    };
}//namespace ngl::tools
