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
		bool readline(std::string& aline);
		bool readcurrent(std::string& astr);
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
		void write(const std::string& aneirong);
	};

	class filetools
	{
		filetools() = delete;
		filetools(const filetools&) = delete;
		filetools& operator=(const filetools&) = delete;
	public:
		// # �Ƿ����ĳ���ļ�
		static bool exist(const std::string& afilename);

		// # �Ƿ����ĳ��·��
		static bool path_exist(const std::string& apath);

		// # �Ƴ�ĳ���ļ�
		static bool remove(const std::string& afilename);

		// # ��ȡdir�������ļ�
		// # aiteration�Ƿ�ѭ������dir�µ�����Ŀ¼
		static void dir(const std::string& apath, std::vector<std::string>& afilevec, bool aiteration = false);
	};
}//namespace ngl