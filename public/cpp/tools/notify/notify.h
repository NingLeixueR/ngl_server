#pragma once
#include <map>
#include <vector>
#include <functional>
#include <list>

namespace ngl
{
	template <typename TNOTIFY>
	struct notify_parm;

	template <typename TNOTIFY>
	class notify
	{
	public:
		using callback = std::function<bool(notify_parm<TNOTIFY>*)>;
	private:
		static std::map<size_t, std::map<int64_t, callback>> m_map;
	public:

		template <typename TDerived, typename TPRAM>
		using Tfun = bool (TDerived::*)(TPRAM*);

		template <typename TDerived, typename T>
		static int rfun(const Tfun<TDerived, T>& afun, TDerived* aTDerived);

		template <typename T>
		static void moverfun(int64_t aid);

		template <typename T>
		static bool execute(T& aparm);

	};

	template <typename TNOTIFY>
	std::map<size_t, std::map<int64_t, std::function<bool(notify_parm<TNOTIFY>*)>>> notify<TNOTIFY>::m_map;

	template <typename TNOTIFY>
	struct notify_parm
	{
		size_t m_notify;
		notify_parm(size_t anotify) :m_notify(anotify) {}

		size_t type() { return m_notify; }

		static void moverfun(int aid);

		template <typename TDerived>
		static int rfun(const typename notify<TNOTIFY>::Tfun<TDerived, TNOTIFY>& afun, TDerived* aTDerived);
	};

	template <typename TNOTIFY>
	template <typename TDerived>
	int notify_parm<TNOTIFY>::rfun(const typename notify<TNOTIFY>::Tfun<TDerived, TNOTIFY>& afun, TDerived* aTDerived)
	{
		return notify<TNOTIFY>::template rfun<TDerived, TNOTIFY>(afun, aTDerived);
	}

	template <typename TNOTIFY>
	void notify_parm<TNOTIFY>::moverfun(int aid)
	{
		return notify<TNOTIFY>::template moverfun<TNOTIFY>(aid);
	}

	template <typename TNOTIFY>
	template <typename TDerived, typename T>
	int notify<TNOTIFY>::rfun(const Tfun<TDerived, T>& afun, TDerived* aTDerived)
	{
		std::map<int64_t, callback>& lmap = m_map[T().type()];
		callback lfun = [afun, aTDerived](notify_parm<TNOTIFY>* aparm)->bool
		{
			return (aTDerived->*afun)((T*)(aparm));
		};

		int64_t lid = lmap.empty() ? 1 : lmap.rbegin()->first + 1;
		lmap.insert(std::make_pair(lid, lfun));
		return lid;
	}

	template <typename TNOTIFY>
	template <typename T>
	void notify<TNOTIFY>::moverfun(int64_t aid)
	{
		std::map<int64_t, callback>& lmap = m_map[T().type()];
		auto itor = lmap.find(aid);
		if (itor == lmap.end())
			return;
		lmap.erase(itor);
	}

	template <typename TNOTIFY>
	template <typename T>
	bool notify<TNOTIFY>::execute(T& aparm)
	{
		auto itor = m_map.find(aparm.type());
		if (itor == m_map.end())
		{
			return false;
		}
		for (auto& vfun : itor->second)
		{
			vfun.second(&aparm);
		}
		return true;
	}

#if defined(WIN32)||defined(WINCE)||defined(WIN64)
# define def_construction(T,...)	T() :notify_parm<T>(typeid(T).hash_code()),##__VA_ARGS__ {}
#else
# define def_construction(T,...)	T() :notify_parm<T>(typeid(T).hash_code()) __VA_OPT__(,) ##__VA_ARGS__ {}
#endif

	struct notify_parm_test : public notify_parm<notify_parm_test>
	{
		def_construction(notify_parm_test, m_test(0));

		int m_test;
	};


	struct notify_parm_test1 : public notify_parm<notify_parm_test1>
	{
		def_construction(notify_parm_test1);

		int m_test;

	};



	
	void test_notify();
}