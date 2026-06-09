#include "JobParser.h"

#include <regex>

std::vector<PrintJob>
JobParser::parse(
    const std::vector<std::string>& messages)
{
    std::unordered_map<int, PrintJob> jobs;

    std::regex re(
        R"(\[Job ([0-9]+)\] argv\[([0-9]+)\]="(.*)");

    std::smatch match;

    for (const auto& msg : messages)
    {
        if (!std::regex_search(msg, match, re))
            continue;

        int jobId =
            std::stoi(match[1]);

        int arg =
            std::stoi(match[2]);

        std::string value =
            match[3];

        auto& job = jobs[jobId];

        job.jobId = jobId;

        switch(arg)
        {
            case 0:
                job.printer = value;
                break;

            case 1:
                job.jobId =
                    std::stoi(value);
                break;

            case 2:
                job.user = value;
                break;

            case 3:
                job.fileName = value;
                break;

            case 4:
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