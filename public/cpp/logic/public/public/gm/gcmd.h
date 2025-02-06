#pragma once

#include "define_json.h"
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
	public:
		T			m_data;
		bool		m_istoutf8 = true;

		gcmd(int aid, const std::string& aoperator) :
			id(aid),
			m_data(),
			m_operator(aoperator)
		{}

		gcmd(int aid, const std::string& aoperator, const T& adata) :
			id(aid),
			m_operator(aoperator),
			m_data(adata)
		{}

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
				pro->add_data()->m_json = ljson;
				actor::static_send_actor(nguid::make_self(ACTOR_GM), -1, pro);
			}
			Catch		
		}
	};
}//namespace ngl