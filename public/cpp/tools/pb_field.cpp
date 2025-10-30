/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "pb_field.h"

namespace ngl
{
    void pb_field::copy(const google::protobuf::Message& src, google::protobuf::Message* dst, const std::set<int>& field_numbers)
    {
        const google::protobuf::Descriptor* desc = src.GetDescriptor();
        const google::protobuf::Reflection* src_refl = src.GetReflection();
        const google::protobuf::Reflection* dst_refl = dst->GetReflection();

        for (int i = 0; i < desc->field_count(); ++i)
        {
            const google::protobuf::FieldDescriptor* field = desc->field(i);
            if (field_numbers.contains(field->number()))
            {
                continue;
            }
            copyfield(src, dst, src_refl, dst_refl, field);
        }
    }

    void pb_field::copyfield(
        const google::protobuf::Message& src
        , google::protobuf::Message* dst
        , const google::protobuf::Reflection* src_refl
        , const google::protobuf::Reflection* dst_refl
        , const google::protobuf::FieldDescriptor* field
    )
    {
        if (field->is_repeated())
        {
            // ���� repeated �ֶΣ��ظ��ֶΣ�
            int size = src_refl->FieldSize(src, field);
            for (int i = 0; i < size; ++i)
            {
                copy_repeated_field(src, dst, src_refl, dst_refl, field, i);
            }
        }
        else
        {
            // ����� repeated �ֶΣ�����ֵ��
            if (src_refl->HasField(src, field))
            {
                copy_single_field(src, dst, src_refl, dst_refl, field);
            }
        }
    }

    // ���Ƶ����� repeated �ֶ�
    void pb_field::copy_single_field(
        const google::protobuf::Message& src
        , google::protobuf::Message* dst
        , const google::protobuf::Reflection* src_refl
        , const google::protobuf::Reflection* dst_refl
        , const google::protobuf::FieldDescriptor* field
    )
    {
        switch (field->cpp_type())
        {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            dst_refl->SetInt32(dst, field, src_refl->GetInt32(src, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            dst_refl->SetInt64(dst, field, src_refl->GetInt64(src, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            dst_refl->SetUInt32(dst, field, src_refl->GetUInt32(src, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            dst_refl->SetUInt64(dst, field, src_refl->GetUInt64(src, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            dst_refl->SetDouble(dst, field, src_refl->GetDouble(src, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            dst_refl->SetFloat(dst, field, src_refl->GetFloat(src, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            dst_refl->SetBool(dst, field, src_refl->GetBool(src, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        {
            const google::protobuf::EnumValueDescriptor* enum_val =
                src_refl->GetEnum(src, field);
            dst_refl->SetEnum(dst, field, enum_val);
            break;
        }
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            dst_refl->SetString(dst, field, src_refl->GetString(src, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        {
            const google::protobuf::Message& src_msg = src_refl->GetMessage(src, field);
            google::protobuf::Message* dst_msg = dst_refl->MutableMessage(dst, field);
            dst_msg->CopyFrom(src_msg); // Ƕ����Ϣȫ������
            break;
        }
        default:
            // �����ϲ����ߵ������������ö��ֵ
            break;
        }
    }

    // ���� repeated �ֶ��еĵ���Ԫ��
    void pb_field::copy_repeated_field(const google::protobuf::Message& src,
        google::protobuf::Message* dst,
        const google::protobuf::Reflection* src_refl,
        const google::protobuf::Reflection* dst_refl,
        const google::protobuf::FieldDescriptor* field,
        int index
    )
    {
        switch (field->cpp_type())
        {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            dst_refl->AddInt32(dst, field, src_refl->GetRepeatedInt32(src, field, index));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            dst_refl->AddInt64(dst, field, src_refl->GetRepeatedInt64(src, field, index));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            dst_refl->AddUInt32(dst, field, src_refl->GetRepeatedUInt32(src, field, index));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            dst_refl->AddUInt64(dst, field, src_refl->GetRepeatedUInt64(src, field, index));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            dst_refl->AddDouble(dst, field, src_refl->GetRepeatedDouble(src, field, index));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            dst_refl->AddFloat(dst, field, src_refl->GetRepeatedFloat(src, field, index));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            dst_refl->AddBool(dst, field, src_refl->GetRepeatedBool(src, field, index));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        {
            const google::protobuf::EnumValueDescriptor* enum_val =
                src_refl->GetRepeatedEnum(src, field, index);
            dst_refl->AddEnum(dst, field, enum_val);
            break;
        }
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            dst_refl->AddString(dst, field, src_refl->GetRepeatedString(src, field, index));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        {
            const google::protobuf::Message& src_msg =
                src_refl->GetRepeatedMessage(src, field, index);
            google::protobuf::Message* dst_msg = dst_refl->AddMessage(dst, field);
            dst_msg->CopyFrom(src_msg); // Ƕ����Ϣȫ������
            break;
        }
        default:
            // ��������ö��ֵ����Ĭ����Ϊ
            break;
        }
    }
}//namespace ngl