#include "StdAfx.h"
#include "FileOperation.h"


void FileOperation::GetAllFormatFiles(std::string& path, std::vector<basic_string<char>>& files, string& format)
{
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		}
		while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

string FileOperation::getFilename(string f_path)
{
	char sep = '/';
	char sepExt = '.';

#ifdef _WIN32
	sep = '\\';
#endif

	size_t i = f_path.rfind(sep, f_path.length());
	if (i != string::npos)
	{
		string fn = (f_path.substr(i + 1, f_path.length() - i));
		size_t j = fn.rfind(sepExt, fn.length());
		if (i != string::npos)
		{
			return fn.substr(0, j);
		}
		else
		{
			return fn;
		}
	}
	else
	{
		return "";
	}
}

int FileOperation::GetDirNumber(string fileNamePath)
{
	long hFile = 0;
	int counter = -2;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(fileNamePath).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib == _A_SUBDIR))
			{
				
				counter++;
				
			}
			else
			{
				counter = counter;
			}
		}
		while (!_findnext(hFile, &fileinfo));
		_findclose(hFile);
	}
	return counter;
}







