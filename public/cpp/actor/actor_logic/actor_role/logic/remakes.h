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

#include <thread>
#include <string>
#include <stack>

namespace ngl
{
	// # remakes
	//  �ڴ������ע��"remakes"�ַ���
	//  ���磺
	//  local_remakes(role, "test_1");
	//  std::string lremakes = local_get_remakes(role); //lremakes == "test_1"
	// {
	//		lremakes = local_get_remakes(role); //lremakes == "test_1"
	//		local_remakes(lremakes, "test_2");
	//		role->addite();
	//		lremakes = local_get_remakes(role); //lremakes == "test_2"
	// }
	//  lremakes = local_get_remakes(role); //lremakes == "test_1"
	class dremakes;
	class remakes
	{
		remakes(const remakes&) = delete;
		remakes& operator=(const remakes&) = delete;

		friend class dremakes;
	private:
		std::stack<std::string> m_remakes;
		void add_remakes(const char* aremakes);
		void erase_remakes();
	public:
		remakes() {}
		const char* get_remakes();
	};

	class actor_role;

	class dremakes
	{
		dremakes(const dremakes&) = delete;
		dremakes& operator=(const dremakes&) = delete;

		actor_role* m_role;
	public:
		dremakes(actor_role* arole, const char* aremakes);
		dremakes(actor_role* arole, const std::string& aremakes);
		virtual ~dremakes();

		static const char* get_remake(actor_role*);
	};
}// namespace ngl

#define local_remakes(ROLE, REMAKES)	ngl::dremakes ldremakes(ROLE, REMAKES)
#define local_get_remakes(ROLE)			ngl::dremakes::get_remake(ROLE)