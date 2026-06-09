#include "JobParser.h"
#include <regex>
#include <iostream>
#include <cassert>
#define assertm(exp, msg) assert((void(msg), exp))
std::vector<PrintJob>
JobParser::parse(
    const std::vector<std::string>& messages)
{
    std::unordered_map<int, PrintJob> jobs;

    
       std::regex re(R"(\[Job ([0-9]+)\] argv\[([0-9]+)\]=(.*))");

    std::smatch match;

    for (const auto& msg : messages)
    {
        if (!std::regex_search(msg, match, re))
            continue;

        std::cout << match[1] << std::endl;
        int jobId = std::stoi(match[1]);
            
        std::cout << match[2] << std::endl;
        int arg =
            std::stoi(match[2]);

        std::cout << match[3] << std::endl;
        std::string value =
            match[3];

        auto& job = jobs[jobId];

        job.jobId = jobId;

        switch(arg)
        {
            case 0:
                std::cout <<"0"<< value << std::endl;
                job.printer = value;
                break;

            case 1:
                std::cout << "1" << value << std::endl;
                job.jobId =
                    std::stoi(value);
                break;

            case 2:
                std::cout << "2" << value << std::endl;
                job.user = value;
                break;

            case 3:
                std::cout << "3" << value << std::endl;
                job.fileName = value;
                break;

            case 4:
                std::cout << "4" << value << std::endl;
                job.copies =
                    std::stoi(value);
                break;
        }
    }

    std::vector<PrintJob> result;

    for (auto& [id, job] : jobs)
    {
        result.push_back(job);
    }

    return result;

}