#include <queue>
#include "graph.h"


class WorkflowSchedule {
private:
    WorkflowGraph graph;
    int numMachines;
public:
    WorkflowSchedule(WorkflowGraph _graph, int _numMachines): graph(_graph), numMachines(_numMachines) {}

    std::vector<std::string> topologicalSort() {
        std::unordered_map<std::string, int> inDegrees = graph.getIndegrees();
        
        std::queue<std::string> q;
        for (const auto& inDeg: inDegrees) {
            if (inDeg.second == 0) {
                q.push(inDeg.first);
            }
        }

        std::vector<std::string> topOrder;
        while (!q.empty()) {
            std::string front = q.front();
            q.pop();
            topOrder.emplace_back(front);

            for (const auto& job: graph.getSuccessors(front)) {
                inDegrees[job.name]--;

                if (inDegrees[job.name] == 0) {
                    q.push(job.name);
                }
            }
        }
        
        return topOrder;
    }

    static int getEarliestMachine(std::vector<int> machineFinishTime) {
        int earliestMachine = 0;
        for (int i = 1; i < machineFinishTime.size(); i++) {
            if (machineFinishTime[i] < machineFinishTime[earliestMachine]) {
                earliestMachine = i;
            }
        }
        return earliestMachine;
    }

    std::pair<int, std::vector<std::string>> schedule() {
        std::vector<std::string> topOrder = topologicalSort();

        std::vector<int> machineFinishTime(numMachines, 0);
        std::unordered_map<std::string, int> jobFinishTime;

        for (const auto& jobName: topOrder) {
            Job currJob = graph.getJob(jobName);
            std::vector<Communication> inCommunications = graph.getInCommunications(jobName);

            int earliestMachine = getEarliestMachine(machineFinishTime);

            int earliestStartTime = 0;
            for (const Communication& comm: inCommunications) {
                auto inJobTimeMap = jobFinishTime.find(comm.fromJob.name);
                int inJobFinishTime = (inJobTimeMap == jobFinishTime.end())? 0: inJobTimeMap->second;
                inJobFinishTime = std::max(inJobFinishTime, machineFinishTime[earliestMachine]) + comm.commTime;

                earliestStartTime = std::max(earliestStartTime, inJobFinishTime);
            }

            machineFinishTime[earliestMachine] = earliestStartTime + currJob.executionTime;
            jobFinishTime[jobName] = earliestStartTime + currJob.executionTime;
        }

        int min_time = *std::max_element(machineFinishTime.begin(), machineFinishTime.end());

        return {min_time, topOrder};
    }
};
