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

#include "session_manage.h"
#include "cmd.h"

namespace ngl
{
	class udp_cmd
	{
	public:
		enum ecmd
		{
			ecmd_connect,				// ��������
			ecmd_connect_ret,			// �����������ߵķ���
			ecmd_ping,					// ��ʱping
			ecmd_close,					// �����Ͽ�����
			ecmd_close_ret,             // �����Ͽ����ӵķ���
			ecmd_minlen = sizeof("ecmd*") - 1,
		};
		using ecmd_callback = std::function<void(asio_kcp*, ptr_se&, const std::string&)>;

		using handle_cmd = cmd<udp_cmd, ecmd, asio_kcp*, ptr_se&, const std::string&>;

		static bool run_cmd(asio_kcp* ap, ptr_se& apstruct, const char* abuf, int32_t alen);

		static void register_fun(ecmd anum, const ecmd_callback& afun);

		static bool sendcmd(asio_kcp* akcp, i32_sessionid asession, ecmd acmd, const std::string& ajson);
	};

}//namespace ngl