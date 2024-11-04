#pragma once

#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <array>
#include <mutex>
#include <cmath>
#include <map>

#include "threadtools.h"
#include "impl.h"

namespace ngl
{
	struct wheel_node;
	class wheel;
	class time_wheel;

	struct wheel_parm
	{
		int64_t									m_timerstart		= 0;		// 定时器开启的时间
		int32_t									m_ms				= 0;		// 首次触发的毫秒
		std::function<int(int64_t)>				m_intervalms		= nullptr;	// 间隔触发的毫秒(根据传递进去的触发时间返回下次触发的间隔)
		int										m_count				= 0;		// 触发次数
		void*									m_pram				= nullptr;	// 透传参数
		std::function<void(const wheel_node*)>	m_fun				= nullptr;	// 定时回调函数
	};

	struct wheel_node
	{
		using timecallback = std::function<void(wheel_node*)>;

		time_wheel*		m_tw;
		int64_t			m_timerid;				// 定时器id
		bool&			m_remove;				// 定时器是否被移除
		wheel_node*		m_next;					// 下一个结点
		wheel_parm		m_parm;					// 定时器回调参数

		wheel_node(time_wheel* atw, int64_t atimerid, const wheel_parm& aparm);

		wheel_node(const wheel_node& aparm);

		wheel_node() = delete;
	};

	struct time_wheel_config
	{
	public:
		int32_t m_time_wheel_precision	= 10;		// 定时器精度  单位毫秒
		int32_t m_time_wheel_bit		= 8;		// 时间轮的槽数  2^etime_wheel_bit
		int32_t m_time_wheel_count		= 4;

		// # 定时器从头建立开始支持的最大时间
		int64_t max_time()
		{
			int64_t lsum = 0;
			int64_t lprecision = m_time_wheel_precision;
			for (int i = 0; i < m_time_wheel_count; ++i)
			{
				lprecision *= std::pow(2, m_time_wheel_bit);
				lsum += lprecision;
			}
			return lsum;
		}

		// # 定时器从头建立开始支持的最大时间(年)
		double year()
		{
			return day() / 365.0;
		}

		// # 定时器从头建立开始支持的最大时间(日)
		double day()
		{
			double lnum = 24 * 60 * 60 * 1000;
			uint64_t lsum = max_time();
			return lsum / lnum;
		}
	};

	class time_wheel
	{
		friend class wheel;

		time_wheel(const time_wheel&) = delete;
		time_wheel& operator=(const time_wheel&) = delete;

		struct impl_time_wheel;
		ngl::impl<impl_time_wheel> m_impl_time_wheel;
	public:
		static int64_t getms();
		time_wheel(const time_wheel_config& aconfig = time_wheel_config(), bool aisthreadcallback = true);
		~time_wheel();

		// # 获取现存定时器数量
		int	count();

		// # 是否没有正在执行的定时器
		bool	empty();

		// # 获取服务器启动的毫秒数
		int64_t server_start_ms();

		// # 获取服务器当前的毫秒数
		int64_t server_current_ms();

		// # addtimer添加定时器
		// # 返回值定时器id   定时器id大于0有效
		int64_t addtimer(const wheel_parm& apram);

		// # removetimer移除指定定时器
		// # int atimerid  定时器id
		void removetimer(int64_t atimerid);

		// # m_isthreadcallback == false
		std::shared_ptr<wheel_node> pop_node();

		bool& get_remove(int64_t atimerid);
	};

	class twheel
	{
		twheel() = delete;
		static time_wheel m_wheel;
	public:
		static time_wheel& wheel()
		{
			return m_wheel;
		}
	};
}//namespace ngl