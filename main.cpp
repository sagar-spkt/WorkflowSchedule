/*GIVEN:
An arbitrary workflow, i.e. directed acyclic graph (DAG) G=(V, E),
where each node v is a job and each directed edge (u, v) shows that
the output data of job u is transferred as the input data of job v,
K homogeneous machines, the execution time t(v) of each job running
individually on a single machine, and the communication time t(u, v)
of each data transfer between jobs u and v running in different machines.

QUESTION:
Find the minimum execution time of the entire workflow along with a
feasible schedule mapping all the jobs onto no more than K machines

REQUIREMENT:
Comment on the difficulty (i.e., the problem's computational complexity)
of the above problem, design an efficient algorithm, analyze your algorithm's
time complexity, implement the solution, (which may be an exact, approximation,
or heuristic algorithm), and show the execution results.
(C/C++ in Linux andMakefile are required.)

I do not expect a perfect solution from you. Based on your source code
and algorithm description, I will evaluate
1) whether you have a correct, clear idea (algorithm) to solve this optimization problem.
2) whether your implementation is correct in source files.
3) your code readability (such as code format and necessary comments), algorithm efficiency, and self-learning capacity.
*/

#include "workflow/all.h"


int main() {
    WorkflowGraph graph;
    graph.addJob("A", 5);
    graph.addJob("B", 3);
    graph.addJob("C", 8);
    graph.addJob("D", 4);
    graph.addJob("E", 2);
    graph.addJob("F", 1);
    graph.addJob("G", 7);
    graph.addJob("H", 3);
    graph.addCommunication("A", "D", 2);
    graph.addCommunication("B", "D", 1);
    graph.addCommunication("C", "D", 5);
    graph.addCommunication("D", "E", 3);
    graph.addCommunication("D", "F", 4);
    graph.addCommunication("E", "G", 1);
    graph.addCommunication("F", "G", 2);
    graph.addCommunication("G", "H", 2);
    graph.printWorkflow();

    WorkflowSchedule schedule(graph, 2);
    auto res = schedule.schedule();
    for (const auto& jobName: res.second) {
        std::cout << jobName << "-->";
    }
    std::cout << "\n\tMin Time: " << res.first;
    return 0;
}
