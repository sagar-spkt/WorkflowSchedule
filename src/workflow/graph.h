#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// Represents a job in the workflow
struct Job {
    std::string name;       ///< Name of the job
    int executionTime;      ///< Time taken by the job for execution

    /**
     * Constructor for the Job struct.
     * @param _name Name of the job
     * @param _executionTime Time taken by the job for execution
     */
    Job(std::string _name, int _executionTime): name(_name), executionTime(_executionTime) {}
};

// Represents communication between two jobs
struct Communication {
    Job* fromJob;           ///< Source job of the communication
    Job* toJob;             ///< Destination job of the communication
    int commTime;           ///< Time taken for communication between jobs

    /**
     * Constructor for the Communication struct.
     * @param _fromJob Source job of the communication
     * @param _toJob Destination job of the communication
     * @param _commTime Time taken for communication between jobs
     */
    Communication(Job* _fromJob, Job* _toJob, int _commTime): fromJob(_fromJob), toJob(_toJob), commTime(_commTime) {}
};

// Represents a directed acyclic graph of jobs and their communications
class WorkflowGraph {
private:
    std::unordered_map<std::string, Job*> jobs;  ///< Map of job names to Job objects
    std::unordered_map<Job*, std::vector<Communication*>> inCommunications;  ///< Map of jobs to their incoming communications
    std::unordered_map<Job*, std::vector<Communication*>> outCommunications;  ///< Map of jobs to their outgoing communications
public:
    /**
     * Destructor for WorkflowGraph class.
     * Deletes dynamically allocated Job and Communication objects.
     */
    ~WorkflowGraph() {
        for (auto& job : jobs) {
            delete job.second;
        }

        for (auto& cpair : inCommunications) {
            for (auto& comm : cpair.second) {
                delete comm;
            }
        }

        for (auto& cpair : outCommunications) {
            for (auto& comm : cpair.second) {
                delete comm;
            }
        }
    }

    /**
     * Adds a new job to the workflow.
     * @param name Name of the job
     * @param executionTime Time taken by the job for execution
     */
    void addJob(std::string name, int executionTime) {
        jobs[name] = new Job(name, executionTime);
        inCommunications[jobs[name]] = std::vector<Communication*>();
        outCommunications[jobs[name]] = std::vector<Communication*>();
    }

    /**
     * Adds a communication link between two jobs.
     * @param fromJobName Name of the source job
     * @param toJobName Name of the destination job
     * @param commTime Time taken for communication between jobs
     */
    void addCommunication(std::string fromJobName, std::string toJobName, int commTime) {
        Communication* newCommunication = new Communication(jobs[fromJobName], jobs[toJobName], commTime);
        inCommunications[jobs[toJobName]].emplace_back(newCommunication);
        outCommunications[jobs[fromJobName]].emplace_back(newCommunication);
    }

    /**
     * Retrieves incoming communications for a given job.
     * @param job The job for which incoming communications are to be retrieved
     * @return Vector of Communication representing incoming communications
     */
    std::vector<Communication*>& getInCommunications(Job* job) {
        return inCommunications[job];
    }

    /**
     * Retrieves outgoing communications for a given job.
     * @param job The job for which outgoing communications are to be retrieved
     * @return Vector of Communication representing outgoing communications
     */
    std::vector<Communication*>& getOutCommunications(Job* job) {
        return outCommunications[job];
    }

    /**
     * Retrieves predecessors (jobs with incoming communications) for a given job.
     * @param job The job for which predecessors are to be retrieved
     * @return Vector of Job representing predecessors
     */
    std::vector<Job*> getPredecessors(Job* job) {
        std::vector<Job*> predJobs;
        for (const auto& comms: getInCommunications(job)) {
            predJobs.emplace_back(comms->fromJob);
        }
        return predJobs;
    }

    /**
     * Retrieves successors (jobs with outgoing communications) for a given job.
     * @param job The job for which successors are to be retrieved
     * @return Vector of Job representing successors
     */
    std::vector<Job*> getSuccessors(Job* job) {
        std::vector<Job*> succJobs;
        for (const auto& comms: getOutCommunications(job)) {
            succJobs.emplace_back(comms->toJob);
        }
        return succJobs;
    }

    /**
     * Calculates indegrees (number of incoming communications) for all jobs in the workflow.
     * @return Map of Job to their respective indegrees
     */
    std::unordered_map<Job*, int> getIndegrees() {
        std::unordered_map<Job*, int> inDegrees;
        for (const auto& cpair: inCommunications) {
            inDegrees[cpair.first] = cpair.second.size();
        }

        return inDegrees;
    }

    /**
     * Prints the workflow, including job names, execution times, and communication details.
     */
    void printWorkflow() {
        for (const auto& job : jobs) {
            std::cout << "Job: " << job.first << " (Execution Time: " << job.second->executionTime << "):\n";
            for (const auto& communication : outCommunications[job.second]) {
                std::cout << "\t-> " << communication->toJob->name << " (Communication Time: " << communication->commTime << ")\n";
            }
            std::cout << std::endl;
        }
    }
};
