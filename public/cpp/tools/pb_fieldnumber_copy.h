#pragma once

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <vector>
#include <unordered_set>

#include "tools.h"

namespace ngl
{
    // �����ֻࣺ����ָ���ֶα�ŵ�����
    class pb_field
    {
    public:
        // ���� src �� dst����ֻ�����ֶα���� field_numbers �е��ֶ�
        // field_numbers����Ҫ���Ƶ��ֶα���б��� {1, 3, 5}��
        static void copy(const google::protobuf::Message& src, google::protobuf::Message* dst, const std::set<int>& field_numbers);

    private:
        // ���Ƶ����ֶΣ������������͵��ֶΣ��������͡�repeated����Ϣ���͵ȣ�
        static void copyfield(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const google::protobuf::Reflection* src_refl
            , const google::protobuf::Reflection* dst_refl
            , const google::protobuf::FieldDescriptor* field
        );

        // ���Ƶ����� repeated �ֶ�
        static void copy_single_field(
            const google::protobuf::Message& src
            , google::protobuf::Message* dst
            , const google::protobuf::Reflection* src_refl
            , const google::protobuf::Reflection* dst_refl
            , const google::protobuf::FieldDescriptor* field
        );

        // ���� repeated �ֶ��еĵ���Ԫ��
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
            // 1. ��ȡ db_brief ��Ϣ����������Descriptor��
            const google::protobuf::Descriptor* db_brief_descriptor = T::descriptor();
            if (db_brief_descriptor == nullptr) 
            {
                tools::no_core_dump();
                return -1;
            }

            // 2. �����ֶ�����"mactivityvalues"����ȡ���ֶε���������FieldDescriptor��
            const google::protobuf::FieldDescriptor* field_descriptor =
                db_brief_descriptor->FindFieldByName(afieldname);
            if (field_descriptor == nullptr) 
            {
                tools::no_core_dump();
                return -1;
            }

            return field_descriptor->number();
        }
    };

}
