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

		// 定义在[actor_gm.cpp]中 		
		void execute(std::shared_ptr<mforward<ngl::np_gm_response>>& apro);

		~gcmd()
		{
			Try
			{
				ngl::json_write lwrite;
				lwrite.write("operator", m_operator);
				lwrite.write("data", m_data);
				lwrite.write("server_name", tools::server_name());
				std::string ljson;
				lwrite.get(ljson);
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