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
// File overview: Declares interfaces for actor gm.

#pragma once

#include "actor/actor_base/actor.h"
#include "tools/tools.h"

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

		// [Actor_gm.cpp]in
		void execute(std::shared_ptr<mforward<ngl::np_gm_response>>& apro);

		~gcmd()
		{
			Try
			{
				ncjson lwrite;
				std::string lservername = tools::server_name();
				njson::push(lwrite, { "operator", "data", "server_name" }, m_operator, m_data, lservername);
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