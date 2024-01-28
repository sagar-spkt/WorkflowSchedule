#include <queue>
#include "graph.h"

/**
 * Functor for comparing jobs based on their priority with respect to criticality.
 * Used in priority_queue for sorting jobs in decreasing order of job criticality.
 */
class JobCriticalityCompare {
private:
    WorkflowGraph* graph;   ///< Pointer to the WorkflowGraph object
    std::unordered_map<Job*, int> jobCriticalWeights;  ///< Maximum sum of job execution and communication time from the job to terminal job
public:
    /**
     * Constructor for JobCriticalityCompare.
     * @param _graph Pointer to the WorkflowGraph object
     */
    JobCriticalityCompare(WorkflowGraph* _graph): graph(_graph) {}

    /**
     * Get the critical weight of a job, considering maximum execution and communication times
     * among all the paths from the specified job to the terminal job.
     * @param job Strating job
     * @return Maximum sum of job execution and communication time from the job to terminal job
     */
    int getJobCriticalWeight(Job* job) {
        // if precalculated return it
        auto jobCriticalWeightPtr = jobCriticalWeights.find(job);
        if (jobCriticalWeightPtr != jobCriticalWeights.end()) {
            return jobCriticalWeightPtr->second;
        }

        int jobCriticalWeight = 0;
        for (const auto& comm: graph->getOutCommunications(job)) {
            int currJobCriticalWeight = comm->commTime + getJobCriticalWeight(comm->toJob);
            if (jobCriticalWeight < currJobCriticalWeight) {
                jobCriticalWeight = currJobCriticalWeight;
            }
        }
        jobCriticalWeight += job->executionTime;
        
        // store so that recalculation can be avoided.
        jobCriticalWeights[job] = jobCriticalWeight;
        return jobCriticalWeight;
    }

    /**
     * Comparison operator for jobs based on criticality.
     * @param j1 Pointer to the first job
     * @param j2 Pointer to the second job
     * @return True if j1 is less critical than j2, false otherwise
     */
    bool operator()(Job* j1, Job* j2) {
        return getJobCriticalWeight(j1) < getJobCriticalWeight(j2);
    }
};

/**
 * Represents a scheduled job, including information about the machine, scheduling time, start time, and finish time.
 */
struct ScheduledJob {
    Job* job;            ///< Pointer to the job
    int machineId;       ///< ID of the machine
    int scheduleTime;    ///< Time when the job is scheduled in the machine
    int startTime;       ///< Start time of job execution
    int finishTime;      ///< Finish time of job execution

    /**
     * Constructor for ScheduledJob.
     * @param _job Pointer to the Job object
     * @param _machineId ID of the machine
     * @param _scheduleTime Time when the job is scheduled
     * @param _startTime Start time of job execution
     * @param _finishTime Finish time of job execution
     */
    ScheduledJob(Job* _job, int _machineId, int _scheduleTime, int _startTime, int _finishTime):
        job(_job), machineId(_machineId), scheduleTime(_scheduleTime), startTime(_startTime), finishTime(_finishTime) {}
};
typedef std::vector<ScheduledJob> ScheduleOrder;

// Represents a schedule for a workflow on multiple machines.
class WorkflowSchedule {
private:
    WorkflowGraph* graph;   ///< Pointer to the WorkflowGraph object
    int numMachines;        ///< Number of machines available for scheduling
public:
    /**
     * Constructor for WorkflowSchedule.
     * @param _graph Pointer to the WorkflowGraph object
     * @param _numMachines Number of machines available for scheduling
     */
    WorkflowSchedule(WorkflowGraph* _graph, int _numMachines): graph(_graph), numMachines(_numMachines) {}

    /**
     * Performs a topological sort of the workflow graph.
     * Among the executable jobs whose all predecessors are completed,
     * it gives priority to the job with highest critical weight. 
     * @return Vector of Job representing the topological order
     */
    std::vector<Job*> topologicalSort() {
        std::unordered_map<Job*, int> inDegrees = graph->getIndegrees();
        
        // Use priority queue so that among the jobs that can be run simultaneously,
        // highest priority job based in the comparator defined below will be scheduled first.
        JobCriticalityCompare comparator = JobCriticalityCompare(graph);
        std::priority_queue<Job*, std::vector<Job*>, JobCriticalityCompare> pq(comparator);
        for (const auto& inDeg: inDegrees) {
            if (inDeg.second == 0) {
                pq.push(inDeg.first);
            }
        }

        std::vector<Job*> topOrder;
        while (!pq.empty()) {
            Job* front = pq.top();
            pq.pop();
            topOrder.emplace_back(front);

            for (const auto& job: graph->getSuccessors(front)) {
                inDegrees[job]--;

                if (inDegrees[job] == 0) {
                    pq.push(job);
                }
            }
        }
        
        return topOrder;
    }

    /**
     * Schedules the workflow on multiple machines and calculates the makespan.
     * Based on the topological order of the graph, job is scheduled in the machine where it'll be finished earlier.
     * @return Pair containing the makespan and a vector of Job along with scheduling information representing the schedule order
     */
    std::pair<int, ScheduleOrder> schedule() {
        ScheduleOrder scheduleOrder;  // Final scheduling result to return
        std::vector<Job*> topOrder = topologicalSort();

        std::vector<int> machineFinishTime(numMachines, 0);
        std::unordered_map<Job*, int> jobFinishTime;
        std::unordered_map<Job*, int> job2machineMap;

        for (const auto& job: topOrder) {

            // Find machine which will finish the current job earliest
            // First find finish time in each machine along with other information
            std::vector<ScheduledJob> candidateSchedules;
            for (int machine = 0; machine<numMachines; machine++) {
                // Earliest start time is maximum of machine finish time and max weight time from predecessors.
                int earliestStartTime = machineFinishTime[machine];
                for (const Communication* comm: graph->getInCommunications(job)) {
                    if (machine == job2machineMap[comm->fromJob]) {
                        // if predecessor job was executed in the same machine, no communication time is needed.
                        continue;
                    }
                    earliestStartTime = std::max(earliestStartTime, jobFinishTime[comm->fromJob] + comm->commTime);
                }

                int earliestFinishTime = earliestStartTime + job->executionTime;
                candidateSchedules.emplace_back(
                    ScheduledJob(job, machine, machineFinishTime[machine], earliestStartTime, earliestFinishTime)
                );
            }

            // Select the best machine and its schedule information
            ScheduledJob& bestSchedule = candidateSchedules.front();
            for (auto sch = candidateSchedules.begin(); sch != candidateSchedules.end(); sch++) {
                if (bestSchedule.finishTime > sch->finishTime) {
                    bestSchedule = *sch;
                }
            }

            // add the schedule in result schedule order
            scheduleOrder.emplace_back(bestSchedule);

            // log info for next job scheduling
            machineFinishTime[bestSchedule.machineId] = bestSchedule.finishTime;
            jobFinishTime[job] = bestSchedule.finishTime;
            job2machineMap[job] = bestSchedule.machineId;
        }

        int makespan = 0;
        for (const int& mTime: machineFinishTime) {
            if (makespan < mTime) {
                makespan = mTime;
            }
        }

        return {makespan, scheduleOrder};
    }
};
