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
		actor*		m_actor = nullptr;
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

		// 定义在[actor_gm.cpp]中 		
		void execute(std::shared_ptr<mforward<ngl::np_gm_response>>& apro);

		~gcmd()
		{
			Try
			{
				ncjson lwrite;
				std::string lservername = tools::server_name();
				njson::push(lwrite.json()
					, { "operator", "data", "server_name" }
					, m_operator
					, m_data
					, lservername
				);
				std::string ljson = lwrite.str();
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