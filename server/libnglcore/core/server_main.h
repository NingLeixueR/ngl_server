#pragma once

// ��ƽ̨����/�����
#ifdef _WIN32
#	ifdef NGL_EXPORT  // ���ɶ�̬��ʱ����
#		define NGL_EXPORT __declspec(dllexport)
#	else
#		define NGL_EXPORT __declspec(dllimport)  // ʹ��ʱ����
#	endif
#else
#	define NGL_EXPORT  // Linux/macOS������������
#endif

NGL_EXPORT int ngl_main(int argc, char** argv);