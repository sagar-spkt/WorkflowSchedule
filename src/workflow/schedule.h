#include <queue>
#include "graph.h"

/**
 * Functor for comparing jobs based on their makespan.
 * Used in priority_queue for sorting jobs in decreasing order of job makespan.
 */
class JobMakespanCompare {
private:
    WorkflowGraph* graph;   ///< Pointer to the WorkflowGraph object
public:
    /**
     * Constructor for JobMakespanCompare.
     * @param _graph Pointer to the WorkflowGraph object
     */
    JobMakespanCompare(WorkflowGraph* _graph): graph(_graph) {}

    /**
     * Comparison operator for jobs based on makespan.
     * @param j1 Pointer to the first job
     * @param j2 Pointer to the second job
     * @return True if makespan of j1 is less than makespan of j2, false otherwise
     */
    bool operator()(Job* j1, Job* j2) {
        return graph->getJobMaxMakespan(j1) < graph->getJobMaxMakespan(j2);
    }
};


struct ScheduledJob {
    Job* job;
    int machineId;
    int scheduleTime;
    int startTime;
    int finishTime;
    ScheduledJob(Job* _job, int _machineId, int _scheduleTime, int _startTime, int _finishTime): job(_job), machineId(_machineId), scheduleTime(_scheduleTime), startTime(_startTime), finishTime(_finishTime) {}
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
     * it gives priority to the job with highest makespan. 
     * @return Vector of Job representing the topological order
     */
    std::vector<Job*> topologicalSort() {
        std::unordered_map<Job*, int> inDegrees = graph->getIndegrees();
        
        // Use priority queue so that among the jobs that can be run simultaneously,
        // highest priority job based in the comparator defined below will be scheduled first.
        JobMakespanCompare comparator = JobMakespanCompare(graph);
        std::priority_queue<Job*, std::vector<Job*>, JobMakespanCompare> pq(comparator);
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
     * Determines the index of the machine with the earliest finish time.
     * @param machineFinishTime Vector containing finish times of each machine
     * @return Index of the machine with the earliest finish time
     */
    static int getEarliestMachine(std::vector<int>& machineFinishTime) {
        int earliestMachine = 0;
        for (int i = 1; i < machineFinishTime.size(); i++) {
            if (machineFinishTime[i] < machineFinishTime[earliestMachine]) {
                earliestMachine = i;
            }
        }
        return earliestMachine;
    }

    /**
     * Schedules the workflow on multiple machines and calculates the makespan.
     * Based on the topological order of the graph, job is scheduled in the earliest available machine.
     * @return Pair containing the makespan and a vector of Job representing the schedule order
     */
    std::pair<int, ScheduleOrder> schedule() {
        ScheduleOrder scheduleOrder;
        std::vector<Job*> topOrder = topologicalSort();

        std::vector<int> machineFinishTime(numMachines, 0);
        std::unordered_map<Job*, int> jobFinishTime;
        std::unordered_map<Job*, int> job2machineMap;

        for (const auto& job: topOrder) {
            int earliestMachine = getEarliestMachine(machineFinishTime);

            int earliestStartTime = machineFinishTime[earliestMachine];
            for (const Communication* comm: graph->getInCommunications(job)) {
                if (earliestMachine == job2machineMap[comm->fromJob]) {
                    // if predecessor job was executed in the same machine, no communication time is needed.
                    continue;
                }
                earliestStartTime = std::max(earliestStartTime, jobFinishTime[comm->fromJob] + comm->commTime);
            }

            int earliestFinishTime = earliestStartTime + job->executionTime;

            scheduleOrder.emplace_back(
                ScheduledJob(job, earliestMachine, machineFinishTime[earliestMachine], earliestStartTime, earliestFinishTime)
            );

            machineFinishTime[earliestMachine] = earliestFinishTime;
            jobFinishTime[job] = earliestFinishTime;
            job2machineMap[job] = earliestMachine;
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
