#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <iostream>
#include <cstring>

namespace ngl
{
	struct rguid
	{
		std::vector<int64_t> m_64vlaues;
		
		rguid(int64_t aid)
		{
			m_64vlaues.push_back(aid);
		}

		rguid(){}

		template <typename T>
		void init(const T& avalue)
		{
			using t1 = std::remove_reference<T>::type;
			using t2 = std::remove_const<t1>::type;
			m_64vlaues.push_back(typeid(t2).hash_code());
		}

		template <typename T, typename ...TARG>
		void init(const T& avalue, const TARG&... arg)
		{
			init(avalue);
			init(arg...);
		}

		template <typename... T>
		void init(const std::tuple<T...>&)
		{
			init(T{}...);
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
		static std::map<rguid, mame_function> m_fun;

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
		static void registers(Tfun<TDerived, TARG...> afun, const char* amessage = "")
		{
			rguid lvalues(typeid(typename std::remove_const<TDerived>::type).hash_code());
			using targ = std::tuple<typename std::remove_reference<TARG>::type...>;
			lvalues.init(targ{});
			m_fun[lvalues] = mame_function
			{
				.m_function	= supercast<void*>(afun),
				.m_message	= amessage
			};
		}

		template <typename TDerived, typename ...TARG>
		static bool handle_switch(TDerived* aeffect, TARG&&... arg)
		{
			rguid lvalues(typeid(typename std::remove_const<TDerived>::type).hash_code());
			lvalues.init(arg...);
			auto itor = m_fun.find(lvalues);
			if (itor == m_fun.end())
				return false;
			std::cout << "handle_switch[" << itor->second.m_message << "]" << std::endl;
			Tfun<TDerived, TARG...> lpfun = supercast<Tfun<TDerived, TARG...>>(itor->second.m_function);
			((aeffect)->*lpfun)(arg...);
			return true;
		}
	};

	void test_rfun();
}