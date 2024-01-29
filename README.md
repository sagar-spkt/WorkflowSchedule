# Workflow Optimization Project

This C++ project is designed to solve the optimization problem of finding the minimum execution time of a directed acyclic graph (DAG) representing a workflow, along with a feasible schedule mapping all the jobs onto a specified number of homogeneous machines.

## Workflow Optimization Algorithm

The project includes an algorithm implementation to find the minimum execution time of the workflow along with a feasible schedule for mapping jobs onto machines. The algorithm uses a topological sort with a priority queue among executable(unblocking) jobs giving higher priority to the job that has maximum critical weight. Here, critical weight of a job is maximum accumulated execution and communication time among all the paths from the specified job to the terminal job. It uses scheduling approach that assigns jobs to the machine that complete it earliest. The communication overhead is assumed zero when linked job are scheduled in same machine.

**[COMPLETE DOCUMENTATION ON ALGORITHM DESIGN AND ANALYSIS CAN BE FOUND HERE](./docs/DESIGN_AND_ANALYSIS.md)**

## Directory Structure

```
.
├── docs
│   └── *
├── .gitignore
├── Makefile
├── README.md
└── src
    ├── main.cpp
    └── workflow
        ├── all.h
        ├── graph.h
        └── schedule.h
```

- **docs/\***: Documentation for algorithm analysis, design, and implementation.
- **.gitignore**: Configuration file for Git to ignore certain files or directories.
- **Makefile**: A makefile for compiling and building the project.
- **README.md**: This file is what you think it is.
- **src**
  - **main.cpp**: The main program demonstrating the workflow optimization problem.
  - **workflow**
    - **all.h**: Header file for including all workflow-related components.
    - **graph.h**: Header file containing the definitions for the workflow graph and related structures.
    - **schedule.h**: Header file with classes for scheduling the workflow on multiple machines.

## Build and Run

To build the project, use the provided `Makefile`. Open a terminal and navigate to the project directory. Then run:

```bash
make
```

This will compile the source code and create an executable named `main` in the `build` directory.

To run the program, execute:

```bash
make run
```

This will execute the compiled `main` program, demonstrating the workflow optimization. The example used in demonstration is of the following workflow graph and 2 homogeneous machine.

![Workflow DAG](./docs/images/Workflow%20Graph.png "Workflow DAG")

And, here is the sample output. For the input graph above, even though two machine are available, the algorithm finds out that executing all task sequentially in a single machine is faster because it avoids the communication cost that is required if executed in separate machines. If you run the program removing some communication links, jobs will be sheduled on multiple machine.

![Workflow output](./docs/images/Sample%20Output.png "Workflow output")

#### Cleaning Up

To clean up the build artifacts, run:

```bash
make clean
```

This will remove the `build` directory.
