#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <vector>

namespace ngl
{
	class readfile
	{
		readfile() = delete;
		readfile(const readfile&) = delete;
		readfile& operator=(const readfile&) = delete;

		std::ifstream m_file;
	public:
		explicit readfile(const std::string& filename);

		~readfile();

		// # �Ƿ�ɹ����ļ�
		bool is_open()const;

		// # ��ȡ�ļ�������
		int get_maxline();

		// # ����ǰanum��
		void jumpbegin(int anum, bool aiscsv);

		// # �ж�ȡ
		bool readline(std::string& aline);

		// # ��ȡ��ǰλ��֮���ȫ������
		bool readcurrent(std::string& astr);

		// # ��ȡ�����ļ�������
		void read(std::string& aneirong);
	};

	class writefile
	{
		writefile() = delete;
		writefile(const writefile&) = delete;
		writefile& operator=(const writefile&) = delete;

		std::ofstream m_file;
	public:
		writefile(const std::string& filename);

		~writefile();

		// # ���ļ�д������
		void write(const std::string& aneirong);
	};
}//namespace ngl