#include <fstream> 
#include <iterator> 
#include <iostream> 
#include <string>
#include <filesystem> 
#include <vector>
#include <regex>
#include <ctime>

struct FileData
{
    std::string path;
    std::string time;
    bool isFailed;
};

std::vector<std::string> GetFilepath()
{
	std::vector<std::string> filepath;

	namespace stdfs = std::filesystem;
	stdfs::path currentPath = stdfs::current_path();
    const stdfs::directory_iterator end{};
	for (stdfs::directory_iterator iter{ currentPath }; iter != end; ++iter)
	{
		auto filename = iter->path().string();
        std::smatch sm;
        std::regex str_expr("\\d{12}_.{7,27}[[:punct:]]txt");
        if (std::regex_search(filename.cbegin(), filename.cend(), sm, str_expr))
		{
			filepath.push_back(filename);
			std::cout << filename << std::endl;
		}
	}
	return filepath;
};

class Parser
{
public:
	std::fstream inputFile;
	std::string content;

    std::vector<FileData> FilesData;
    FileData SData;

	bool IsError(const std::string &inputLine)
	{
        return inputLine.find("Error") != std::string::npos;
	}

	void Parse(std::vector<std::string> path)
	{
		bool errorFlag = false;
		bool contentFlag = false;
		for (const auto& filename : path)
		{
            SData.path = filename;
			inputFile.open(filename);
			while (getline(inputFile, content))
			{
				auto contentPosition = content.find("Detected device");
                if (contentPosition != std::string::npos)
                {
                    std::smatch match_time;
                    std::regex time_expr("(\\d{1,10}[[:punct:]]\\d{1,10})");
                    if (std::regex_search(content.cbegin(), content.cend(), match_time, time_expr))
                    {
                        SData.time = match_time[1];
                        contentFlag = true;
                    }
                    if (errorFlag)
                        SData.isFailed = true;
					else
                        SData.isFailed = false;
                    continue;
				}
                if (IsError(content))
                    errorFlag = true;
			}
			if (contentFlag == false && errorFlag == true)
			{
                SData.time = "Error";
                SData.isFailed = true;
			}
			contentFlag = false;
			errorFlag = false;
			inputFile.close();
            FilesData.push_back(SData);
		}
	}
};

std::fstream CreateFile()
{
	std::fstream file;
	file.open("Time.csv", std::fstream::in | std::fstream::out | std::ostream::trunc);
	return file;
}

void FillFile(std::fstream file, Parser &obj)
{
	file << "Connection time" << ',' << "Path" << ',' << "isError?" << '\n';
    for (int i = 0; i < obj.FilesData.size(); ++i)
	{
        file << obj.FilesData[i].time << ',' << obj.FilesData[i].path << ',' << obj.FilesData[i].isFailed << '\n';
	}
	file.close();
}

int main()
{
	Parser obj;
	obj.Parse(GetFilepath());
	FillFile(CreateFile(), obj);
	return 0;
}