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
    std::unordered_map<Job*, std::vector<Communication*>> communications;  ///< Map of jobs to their outgoing communications
public:
    /**
     * Destructor for WorkflowGraph class.
     * Deletes dynamically allocated Job and Communication objects.
     */
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

    /**
     * Adds a new job to the workflow.
     * @param name Name of the job
     * @param executionTime Time taken by the job for execution
     */
    void addJob(std::string name, int executionTime) {
        jobs[name] = new Job(name, executionTime);
        communications[jobs[name]] = std::vector<Communication*>();
    }

    /**
     * Adds a communication link between two jobs.
     * @param fromJobName Name of the source job
     * @param toJobName Name of the destination job
     * @param commTime Time taken for communication between jobs
     */
    void addCommunication(std::string fromJobName, std::string toJobName, int commTime) {
        communications[jobs[fromJobName]].emplace_back(new Communication(jobs[fromJobName], jobs[toJobName], commTime));
    }

    /**
     * Retrieves incoming communications for a given job.
     * @param job The job for which incoming communications are to be retrieved
     * @return Vector of Communication representing incoming communications
     */
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

    /**
     * Retrieves outgoing communications for a given job.
     * @param job The job for which outgoing communications are to be retrieved
     * @return Vector of Communication representing outgoing communications
     */
    std::vector<Communication*>& getOutCommunications(Job* job) {
        return communications[job];
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

    /**
     * Calculates the maximum makespan for a given job.
     * It is equal to job execuation time plus the maximum outgoing communication time
     * @param job The job for which the maximum makespan is to be calculated
     * @return The maximum makespan for the given job
     */
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

    /**
     * Prints the workflow, including job names, execution times, and communication details.
     */
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
