#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>


struct Job {
    std::string name;
    int executionTime;
    Job() {}
    Job(std::string _name, int _executionTime): name(_name), executionTime(_executionTime) {}
};


struct Communication {
    Job fromJob;
    Job toJob;
    int commTime;
    Communication() {}
    Communication(Job _fromJob, Job _toJob, int _commTime): fromJob(_fromJob), toJob(_toJob), commTime(_commTime) {}
};


class WorkflowGraph {
private:
    std::unordered_map<std::string, Job> jobs;
    std::unordered_map<std::string, std::vector<Communication>> communications;
public:
    void addJob(std::string name, int executionTime) {
        jobs[name] = Job(name, executionTime);
        communications[name] = std::vector<Communication>();
    }

    void addCommunication(std::string fromJobName, std::string toJobName, int commTime) {
        communications[fromJobName].emplace_back(Communication(jobs[fromJobName], jobs[toJobName], commTime));
    }

    Job getJob(std::string name) {
        return jobs[name];
    }

    std::vector<Communication> getInCommunications(std::string jobName) {
        std::vector<Communication> inCommunications;
        for (const auto& job: jobs) {
            for (const auto& comm: communications[job.first]) {
                if (comm.toJob.name == jobName) {
                    inCommunications.emplace_back(comm);
                }
            }
        }
        return inCommunications;
    }

    std::vector<Communication> getOutCommunications(std::string jobName) {
        std::vector<Communication> outCommunications;
        for (const auto& comm: communications[jobName]) {
            outCommunications.emplace_back(comm);
        }
        return outCommunications;
    }

    std::vector<Job> getPredecessors(std::string jobName) {
        std::vector<Job> predJobs;
        for (const auto& comms: getInCommunications(jobName)) {
            predJobs.emplace_back(comms.fromJob);
        }
        return predJobs;
    }

    std::vector<Job> getSuccessors(std::string jobName) {
        std::vector<Job> succJobs;
        for (const auto& comms: getOutCommunications(jobName)) {
            succJobs.emplace_back(comms.toJob);
        }
        return succJobs;
    }

    std::unordered_map<std::string, int> getIndegrees() {
        std::unordered_map<std::string, int> inDegrees;
        for (const auto& job: jobs) {
            inDegrees[job.first] = 0;
        }

        for (const auto& comm: communications) {
            for (const auto& j: comm.second) {
                inDegrees[j.toJob.name]++;
            }
        }

        return inDegrees;
    }

    void printWorkflow() {
        for (const auto& job : jobs) {
            std::cout << "Job: " << job.first << " (Execution Time: " << job.second.executionTime << "):\n";
            for (const auto& communication : communications[job.first]) {
                std::cout << "\t-> " << communication.toJob.name << " (Communication Time: " << communication.commTime << ")\n";
            }
            std::cout << std::endl;
        }
    }
};
