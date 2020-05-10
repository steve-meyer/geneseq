# geneseq Examples

## Max

After building the geneseq object from source, you can open the Max patch examples:

* **overview.maxpat** a simple overview of inputs/outputs
* **genesequencer.maxpat** generate MIDI notes from a geneseq object for two alternating sequences

## Python

The python examples are included as a simple object-oriented example of the geneseq logic. Prior to writing the C code for the object, the python code was adapted to work out the logic without concern for C pointers and lower level data structures.

Run the python code:

```bash
$ cd examples/python
$ python mutate_melody.py
Generation: 1 (2)	Best Chromosome: 16664205	Fitness: 7
Generation: 2 (5)	Best Chromosome: 16664205	Fitness: 7
Generation: 3 (11)	Best Chromosome: 12865705	Fitness: 6
Generation: 4 (24)	Best Chromosome: 12765705	Fitness: 6
Generation: 5 (51)	Best Chromosome: 12255755	Fitness: 5
Generation: 6 (108)	Best Chromosome: 12271055	Fitness: 4
Generation: 7 (64)	Best Chromosome: 12650005	Fitness: 4
Generation: 8 (64)	Best Chromosome: 12835450	Fitness: 3
Generation: 9 (64)	Best Chromosome: 12840000	Fitness: 3
Generation: 10 (64)	Best Chromosome: 12260050	Fitness: 2
Generation: 11 (64)	Best Chromosome: 12830750	Fitness: 2
Generation: 12 (64)	Best Chromosome: 12530750	Fitness: 2
Generation: 13 (64)	Best Chromosome: 12060050	Fitness: 1
Generation: 14 (64)	Best Chromosome: 12230050	Fitness: 1
Generation: 15 (64)	Best Chromosome: 12230050	Fitness: 1
Generation: 16 (64)	Best Chromosome: 12030050	Fitness: 0
```
