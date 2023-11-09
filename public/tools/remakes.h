#pragma once

#include <thread>
#include <stack>
#include <string>

namespace ngl
{
	class dremakes;
	class remakes
	{
		friend class dremakes;
	private:
		std::stack<std::string> m_remakes;
		void add_remakes(const char* aremakes);
		void erase_remakes();
	public:
		const char* get_remakes();

		
	};

	class actor_role;


	class dremakes
	{
		actor_role* m_role;
	public:
		dremakes(actor_role* arole, const char* aremakes);
		dremakes(actor_role* arole, const std::string& aremakes);
		virtual ~dremakes();

		static const char* get_remake(actor_role*);
	};


}

#define d_remakes(ROLE, REMAKES)	ngl::dremakes ldremakes(ROLE, REMAKES)
#define g_remakes(ROLE)				ngl::dremakes::get_remake(ROLE)