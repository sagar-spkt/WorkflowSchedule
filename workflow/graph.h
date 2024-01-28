#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>


struct Job {
    std::string name;
    int executionTime;
    Job(std::string _name, int _executionTime): name(_name), executionTime(_executionTime) {}
};


struct Communication {
    Job* fromJob;
    Job* toJob;
    int commTime;
    Communication(Job* _fromJob, Job* _toJob, int _commTime): fromJob(_fromJob), toJob(_toJob), commTime(_commTime) {}
};


class WorkflowGraph {
private:
    std::unordered_map<std::string, Job*> jobs;
    std::unordered_map<Job*, std::vector<Communication*>> communications;
public:
    ~WorkflowGraph() {
        for (auto& job : jobs) {
            delete job.second;
        }

        for (auto& cpair : communications) {
            for (auto& comm : cpair.second) {
                delete comm;
            }
        }
    }

    void addJob(std::string name, int executionTime) {
        jobs[name] = new Job(name, executionTime);
        communications[jobs[name]] = std::vector<Communication*>();
    }

    void addCommunication(std::string fromJobName, std::string toJobName, int commTime) {
        communications[jobs[fromJobName]].emplace_back(new Communication(jobs[fromJobName], jobs[toJobName], commTime));
    }

    std::vector<Communication*> getInCommunications(Job* job) {
        std::vector<Communication*> inCommunications;
        for (const auto& jpair: jobs) {
            for (const auto& comm: communications[jpair.second]) {
                if (comm->toJob == job) {
                    inCommunications.emplace_back(comm);
                }
            }
        }
        return inCommunications;
    }

    std::vector<Communication*>& getOutCommunications(Job* job) {
        return communications[job];
    }

    std::vector<Job*> getPredecessors(Job* job) {
        std::vector<Job*> predJobs;
        for (const auto& comms: getInCommunications(job)) {
            predJobs.emplace_back(comms->fromJob);
        }
        return predJobs;
    }

    std::vector<Job*> getSuccessors(Job* job) {
        std::vector<Job*> succJobs;
        for (const auto& comms: getOutCommunications(job)) {
            succJobs.emplace_back(comms->toJob);
        }
        return succJobs;
    }

    std::unordered_map<Job*, int> getIndegrees() {
        std::unordered_map<Job*, int> inDegrees;
        for (const auto& job: jobs) {
            inDegrees[job.second] = 0;
        }

        for (const auto& comm: communications) {
            for (const auto& j: comm.second) {
                inDegrees[j->toJob]++;
            }
        }

        return inDegrees;
    }

    int getJobMaxMakespan(Job* job) {
        int time = 0;
        for(const auto& comm: getOutCommunications(job)) {
            if (comm->commTime > time) {
                time = comm->commTime;
            }
        }
        time += job->executionTime;
        return time;
    }

    void printWorkflow() {
        for (const auto& job : jobs) {
            std::cout << "Job: " << job.first << " (Execution Time: " << job.second->executionTime << "):\n";
            for (const auto& communication : communications[job.second]) {
                std::cout << "\t-> " << communication->toJob->name << " (Communication Time: " << communication->commTime << ")\n";
            }
            std::cout << std::endl;
        }
    }
};
