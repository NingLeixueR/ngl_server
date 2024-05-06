#pragma once

#include "template_arg.h"

#include <functional>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <string>
#include <map>

namespace ngl
{
	struct rguid
	{
		std::vector<int64_t> m_64vlaues;
		
		rguid()
		{
		}

		class _handle
		{
		public:
			template <typename T>
			static void func(rguid& arguid)
			{
				using t1 = std::remove_reference<T>::type;
				using t2 = std::remove_const<t1>::type;
				arguid.m_64vlaues.push_back(typeid(t2).hash_code());
			}
		};
		using type_handle = template_arg<rguid::_handle, rguid&>;


		template <typename T>
		void push()
		{
			using t1 = std::remove_reference<T>::type;
			using t2 = std::remove_const<t1>::type;
			m_64vlaues.push_back(typeid(t2).hash_code());
		}

		template <typename T, typename ...TARG>
		void push()
		{
			push<T>();
			push<TARG...>();
		}

		bool operator<(const rguid& r)const
		{
			int lsize = m_64vlaues.size();
			int rsize = r.m_64vlaues.size();
			if (lsize < rsize)
				return true;
			else if (lsize > rsize)
				return false;
			else
				return memcmp(&m_64vlaues[0], &r.m_64vlaues[0], lsize * sizeof(int64_t)) < 0;
		}
	};

	class rfuns
	{
	private:
		struct mame_function
		{
			void* m_function = nullptr;		// 类成员函数
			std::string m_message;			// 方便调试查看
		};
		std::map<rguid, mame_function> m_fun;

		template <typename T, typename F>
		static T supercast(F f)
		{
			union FT
			{
				T   t;
				F   f;
			};
			FT ft;
			ft.f = f;
			return ft.t;
		}

		template <typename TDerived, typename ...TARG>
		using Tfun = void (TDerived::*)(TARG...);
	
	public:
		template <typename TDerived, typename ...TARG>
		void registers(Tfun<TDerived, TARG...> afun, const char* amessage = "")
		{
			rguid lguid;
			rguid::type_handle::func<
				TDerived
				, TARG...
			>(lguid);

			m_fun[lguid] = mame_function
			{
				.m_function	= supercast<void*>(afun),
				.m_message	= amessage
			};
		}

		template <typename TDerived, typename ...TARG>
		bool handle_switch(TDerived* aeffect, TARG... arg)
		{
			rguid lguid;
			rguid::type_handle::func<
				TDerived
				, TARG...
			>(lguid);

			auto itor = m_fun.find(lguid);
			if (itor == m_fun.end())
				return false;
			std::cout << "handle_switch[" << itor->second.m_message << "]" << std::endl;
			Tfun<TDerived, const TARG&...> lpfun = supercast<Tfun<TDerived, const TARG&...>>(itor->second.m_function);
			((aeffect)->*lpfun)(arg...);
			return true;
		}
	};

	void test_rfun();
}// namespace ngl