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
#include "tprotocol.h"
#include "nprotocol.h"
#include "ntimer.h"
#include "net.pb.h"
#include "actor.h"

namespace ngl
{
	std::map<size_t, tprotocol::info> tprotocol::m_keyval;
	std::map<i32_protocolnum, tprotocol::info*> tprotocol::m_protocol;
	std::map<std::string, tprotocol::info*> tprotocol::m_nameprotocol;
	int32_t tprotocol::m_customs = 200000000;
}//namespace ngl