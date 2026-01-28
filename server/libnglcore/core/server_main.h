#pragma once

#include <functional>
#include <string>

// 跨平台导出/导入宏
#ifdef _WIN32
#	ifdef NGL_EXPORT  // 生成动态库时定义
#		define NGL_EXPORT __declspec(dllexport)
#	else
#		define NGL_EXPORT __declspec(dllimport)  // 使用时导入
#	endif
#else
#	define NGL_EXPORT  // Linux/macOS无需特殊声明
#endif

NGL_EXPORT int ngl_main(int argc, char** argv);

NGL_EXPORT std::function<void()> dump_logic(const std::string& atitle);