# Fiduccia-Mattheyses
An implementation of the FM partitioning algorithm. We also add a genetic algorithm to improve individual FM iterations.

Use `make run` to test the code yourself.

If this repository is helpful to you please remember to cite the project write-up in your work. Thank you!

`Kowalski, Jan Stefan. A Genetic Addition to Fiduccia-Mattheyses. 3 Oct. 2020.`  
`@misc{Kowalski_2020, title={A genetic addition to Fiduccia-Mattheyses}, author={Kowalski, Jan Stefan}, year={2020}, month={Oct}} `

## Background

The notions of  cells and nets are essential to understanding this code. Cells represent VLSI circuits that occupy area on a chip. Nets represent the interconnect between cells, connecting inputs and outputs.
In graph theory terms, cells are nodes and nets are hypergraphs (generalized edges). Cells and nets exist in a many-to-many relationship; that is, nets have at least one cell and cells are a part of at least one net. 

The partitioning problem looks to efficiently divide a collection of cells into two collections such that the number of connections/nets between the two partitions is minimized. F and M also introduced the notion of a balance ratio, dividing the areas to a user specification. This is accomplished within a tolerance.

This code is then tested on the ISPD98 suite of testbenching circuits, stored in .are and .netD format.<sup>1</sup> 
(https://vlsicad.ucsd.edu/UCLAWeb/cheese/ispd98.html) 

## Abstract
The main objective is to show a linear increase in time with the number of cell-net connections. FM was significant because it was improvement from the O(2^n) results of a naive approach. From there we add increasingly sophisticated mechanisms to improve the final cutstate.

## User Options
All user options are defined as global constants in one of two header files. 
- `include/main.h` for the FM algorithm, printing, and repetition options
- `include/genetic_algorithm` for the genetic algorithm parameters.

There are five parameters in the GA: population size, population culling threshold, mutation frequency,  recombination frequency, and the number of GA passes. 


Details about these options are expanded on in the files.
To get started, try setting `RUN_DEMO_WITH_TESTDATA` to `NO`

---

[1]: As a footnote, this is not necessarily how circuits are stored in modern industry. Moreover, it is assumed that there is only one connection between a cell and any particular net (this may not hold for real circuits).

