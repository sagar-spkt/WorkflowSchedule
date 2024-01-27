# Workflow Optimization Project

This C++ project is designed to solve the optimization problem of finding the minimum execution time of a directed acyclic graph (DAG) representing a workflow, along with a feasible schedule mapping all the jobs onto a specified number of homogeneous machines.

## Workflow Optimization Algorithm

The project includes an algorithm implementation to find the minimum execution time of the workflow along with a feasible schedule for mapping jobs onto machines. The algorithm uses a topological sort with a priority queue among executable(unblocking) jobs giving higher priority to the job that takes maximum time to execute and communicate with successor jobs. It uses scheduling approach that assigns jobs to the machine available earliest.

## Directory Structure

```
.
├── .gitignore
├── main.cpp
├── Makefile
└── workflow
    ├── all.h
    ├── graph.h
    └── schedule.h
```

- **.gitignore**: Configuration file for Git to ignore certain files or directories.
- **main.cpp**: The main program demonstrating the workflow optimization problem.
- **Makefile**: A makefile for compiling and building the project.
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

This will execute the compiled `main` program, demonstrating the workflow optimization.

## Cleaning Up

To clean up the build artifacts, run:

```bash
make clean
```

This will remove the `build` directory.
