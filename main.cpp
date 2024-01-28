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
(C/C++ in Linux and Makefile are required.)

I do not expect a perfect solution from you. Based on your source code
and algorithm description, I will evaluate
1) whether you have a correct, clear idea (algorithm) to solve this optimization problem.
2) whether your implementation is correct in source files.
3) your code readability (such as code format and necessary comments), algorithm efficiency, and self-learning capacity.
*/

#include <iostream>
#include "workflow/all.h"

/**
 * Main function for executing the workflow scheduling example.
 * Creates a WorkflowGraph, adds jobs and communications, prints the workflow,
 * and then schedules the workflow using WorkflowSchedule and prints the result.
 * @return 0 on successful execution
 */
int main() {
    // Create a WorkflowGraph object
    WorkflowGraph graph;

    // Add jobs and communications to the workflow graph
    graph.addJob("A", 5);
    graph.addJob("B", 4);
    graph.addJob("C", 4);
    graph.addJob("D", 7);
    graph.addJob("E", 2);
    graph.addJob("F", 9);
    graph.addJob("G", 6);
    graph.addJob("H", 3);
    graph.addJob("I", 8);
    graph.addCommunication("A", "D", 5);
    graph.addCommunication("A", "F", 3);
    graph.addCommunication("B", "E", 4);
    graph.addCommunication("B", "F", 2);
    graph.addCommunication("C", "B", 2);
    graph.addCommunication("C", "G", 6);
    graph.addCommunication("D", "E", 9);
    graph.addCommunication("D", "H", 1);
    graph.addCommunication("D", "I", 8);
    graph.addCommunication("E", "F", 4);
    graph.addCommunication("F", "G", 2);
    graph.addCommunication("H", "I", 5);
    graph.addCommunication("I", "C", 7);
    graph.addCommunication("I", "F", 1);

    // Print the workflow
    std::cout << "Workflow Graph:\n";
    graph.printWorkflow();

    // Create a WorkflowSchedule object with 2 machines
    WorkflowSchedule schedule(&graph, 2);

    // Schedule the workflow and get the result
    auto res = schedule.schedule();

    // Print the scheduled order of jobs and the minimum time
    std::cout << "Scheduled Order:\n";
    for (const auto& scheduledJob: res.second) {
        std::cout << "\t\"" << scheduledJob.job->name << "\" at machine " << scheduledJob.machineId << " at time " << scheduledJob.startTime;
        std::cout << "\t" << std::endl;
    }
    std::cout << "\nMinimum time to complete workflow: " << res.first << std::endl;

    return 0;
}
