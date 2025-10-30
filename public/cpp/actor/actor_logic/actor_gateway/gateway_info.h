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

#include "nprotocol.h"
#include "nguid.h"

#include <map>

namespace ngl
{
	class gateway_info
	{
		gateway_info(const gateway_info&) = delete;
		gateway_info& operator=(const gateway_info&) = delete;

		// key:area  value.key:roleid
		std::map<i16_area, std::map<i32_actordataid, gateway_socket>> m_info;
		// key:socket
		std::map<i32_socket, gateway_socket*> m_sockinfo;
	public:
		gateway_info() {}

		void updata(const gateway_socket& ainfo);

		bool updata_socket(i16_area aarea, i32_actordataid aactordataid, i32_socket asocket);

		std::map<i16_area, std::map<i32_actordataid, gateway_socket>>& info();

		// # ֻ�ǶϿ�����
		void remove_socket(i32_socket asocket);

		// # ɾ��������Ϣ
		void remove_actorid(i64_actorid aactorid);

		// # ����area��dataid��ȡgateway_socket
		gateway_socket* get(i16_area aarea, i32_actordataid aroleid);

		// # ����socket��ȡgateway_socket
		gateway_socket* get(i32_socket asocket);

		// # ����actorid��ȡgatewayid
		int64_t gatewayid(i64_actorid aid);

		void foreach(const std::function<void(gateway_socket*)>& afun);

		void updata(const np_actor_gatewayinfo_updata& adata);
	};
}//namespace ngl