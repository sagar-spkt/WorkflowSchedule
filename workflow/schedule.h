#include <queue>
#include "graph.h"


class JobMakespanCompare {
private:
    WorkflowGraph* graph;
public:
    JobMakespanCompare(WorkflowGraph* _graph): graph(_graph) {}

    bool operator()(Job* j1, Job* j2) {
        return graph->getJobMaxMakespan(j1) < graph->getJobMaxMakespan(j2);
    }
};


class WorkflowSchedule {
private:
    WorkflowGraph* graph;
    int numMachines;
public:
    WorkflowSchedule(WorkflowGraph* _graph, int _numMachines): graph(_graph), numMachines(_numMachines) {}

    std::vector<Job*> topologicalSort() {
        std::unordered_map<Job*, int> inDegrees = graph->getIndegrees();
        
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

    static int getEarliestMachine(std::vector<int>& machineFinishTime) {
        int earliestMachine = 0;
        for (int i = 1; i < machineFinishTime.size(); i++) {
            if (machineFinishTime[i] < machineFinishTime[earliestMachine]) {
                earliestMachine = i;
            }
        }
        return earliestMachine;
    }

    std::pair<int, std::vector<Job*>> schedule() {
        std::vector<Job*> topOrder = topologicalSort();

        std::vector<int> machineFinishTime(numMachines, 0);
        std::unordered_map<Job*, int> jobFinishTime;

        for (const auto& job: topOrder) {
            int earliestMachine = getEarliestMachine(machineFinishTime);

            int earliestStartTime = machineFinishTime[earliestMachine];
            for (const Communication* comm: graph->getInCommunications(job)) {
                earliestStartTime = std::max(earliestStartTime, jobFinishTime[comm->fromJob] + comm->commTime);
            }

            machineFinishTime[earliestMachine] = earliestStartTime + job->executionTime;
            jobFinishTime[job] = earliestStartTime + job->executionTime;
        }

        int makespan = 0;
        for (const int& mTime: machineFinishTime) {
            if (makespan < mTime) {
                makespan = mTime;
            }
        }

        return {makespan, topOrder};
    }
};
