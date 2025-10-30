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

#include "actor.h"
#include "tools.h"

namespace ngl
{
	template <typename T>
	struct gcmd
	{
	private:
		gcmd() = delete;
		gcmd(const gcmd&) = delete;
		gcmd& operator=(const gcmd&) = delete;
		int			id = 0;
		std::string m_operator;
		actor*		m_actor;
	public:
		T			m_data;
		bool		m_istoutf8 = true;

		gcmd(int aid, const std::string& aoperator, actor* aactor = nullptr) :
			id(aid),
			m_data(),
			m_operator(aoperator),
			m_actor(aactor)
		{}

		gcmd(int aid, const std::string& aoperator, const T& adata, actor* aactor = nullptr) :
			id(aid),
			m_operator(aoperator),
			m_data(adata),
			m_actor(aactor)
		{}

		// ������[actor_gm.cpp]�� 		
		void execute(std::shared_ptr<mforward<ngl::np_gm_response>>& apro);

		~gcmd()
		{
			Try
			{
				ngl::njson_write lwrite;
			std::string lservername = tools::server_name();
				njson::write(lwrite
					, "operator", m_operator
					, "data", m_data
					, "server_name", lservername
				);
				std::string ljson = lwrite.get();
				if (m_istoutf8)
				{
					ngl::tools::to_utf8(ljson, ljson);
				}
				auto pro = std::make_shared<mforward<ngl::np_gm_response>>(id);
				pro->data()->m_json = ljson;
				if (m_actor == nullptr)
				{
					actor::send_actor(nguid::make_self(ACTOR_GM), nguid::make(), pro);
				}
				else
				{
					execute(pro);
				}
			}
			Catch		
		}
	};
}//namespace ngl