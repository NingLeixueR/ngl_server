#pragma once

#include <functional>
#include <string>

// 跨平台导出/导入宏
#ifdef _WIN32
#	ifdef NGLCORE_EXPORTS  // 生成动态库时定义
#		define NGL_EXPORT __declspec(dllexport)
#	else
#		define NGL_EXPORT __declspec(dllimport)  // 使用时导入
#	endif
#else
#	define NGL_EXPORT  // Linux/macOS无需特殊声明
#endif

enum class startup_error : int
{
	ok = 0,
	invalid_args = 2,
	invalid_node_type = 3,
	config_not_found = 4,
	config_load_failed = 5,
	tab_server_missing = 6,
	net_config_missing = 7,
	init_server_failed = 8,
	node_start_failed = 9,
};

NGL_EXPORT int ngl_main(int argc, char** argv);

NGL_EXPORT std::function<void()> dump_logic(const std::string& acontent);
