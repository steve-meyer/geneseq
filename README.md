# geneseq

A genetic algorithm melody generator/sequencer for Max/MSP.

## Building & Installing

You can build this object if you are comfortable building a Max external on your local system. See the [Max SDK from Cycling 74's website](https://cycling74.com/downloads/sdk).

## Known Issues

This Max external is a work in progress and is currently buggy. It has been known to crash Max and M4L/Ableton Live when copying an instance of the object.

## About

This work is an attempt to build a melody sequencer based on a genetic algorithm. It was inspired by the [Generative sequencers thread from the lines community](https://llllllll.co/t/generative-sequencers/19155).

### Overview

A genetic algorithm "evolves" towards a target state, in this case a target melody specified as scale degrees. Evolution is based on a population of individuals. Think of a population of robots "improvising" by each generating different random melodies. The robots are trying to resolve to a target melody. The robot with a melody most similar to the target is chosen for each evolution/generation for the population as the current sequence. When at least one robot produces the target melody, the genetic algorithm has converged and ceases to evolve. Evolution is a process that happens by sending the Max object an "evolve" message.

At present, the following fixed properties are in place:

* Population size: 64 individuals
* Sequence length: 8 steps
* Genes (i.e., the nucleotides): integers 0-8
* Parents per bred individual: 2
* Default target melody: 1 2 0 3 0 0 5 0

### Individuals

An individual is a member of the population. It is a melody generated from the list of valid genes.

The initial population of individuals is seeded from random combinations of genes.

### Genes

The valid genes that can make up an individual are the integers 0-8 inclusive. 0 is intended to represent a skipped sequence step, or silence. Numbers 1-8 are scale degrees root/tonic (1) up to the note one octave above (8). For example, a gene value of 5 is intended to be the 5th scale degree.

### Scores

Individuals are scored so that the best melody can be chosen for a given generation. This individual sequence will be sent out the first/left outlet of the object, along with its fitness score sent out the second outlet.

Individuals are "scored" in two ways. First, they have an integer fitness score based on how many steps they have in common with the current target sequence. A lower score is more fit and closer to the target. Second, they have a random score as a float generated between 0 and 1. The purpose of the random score is to add some variation to the sequencer output when choosing a best candidate individual at a particular point in time. For example, given the following two individuals:

```
1 8 0 3 0 0 5 0
1 2 0 8 0 0 5 0
```

both would have a fitness score of 1 using the default target melody above since each is off by only one step (the 8 in each sequence). The random score is used so that for some generations, the first is chose and in other generations the second is chosen if they are the two lowest by fitness score alone.

### Evolution of Generations

Generation 0 is a random seeded population when the Max object is created. Subsequent generations evolve from the current generation when the geneseq object receives the message "evolve" in its left inlet.

When evolution happens, the 10% most fit individuals of the current generation are automatically added to the next generation. The remaining members of the population are "bred" from the 50% most fit members of the population. In this promiscuous community, the parents are selected at random, but no parent breeds with itself. Bred individuals get genes randomly according to the following (rough) percentages. For each step in the new individual's sequence:

* 45% chance of receiving parent 1's scale degree at the current step
* 45% chance of receiving parent 2's scale degree at the current step
* 10% chance of a random mutation occurring: random gene is chosen

Individuals are always scored when they are created and once all individuals have been created for a given evolutionary cycle, the entire population of individuals is sorted. An individual is sorted by the sum of its fitness score and random scores.

The top scored individual becomes the next sequence set out the left outlet. It is possible that for successive generations the same sequence is output. This is common as an evolutionary cycle approaches convergence.

### Convergence

The sequencer evolves its population when it receives an "evolve" message. When at least one melody matches the current target sequence, it converges. A bang is sent out the rightmost outlet on convergence. This happens the first time an "evolve" message results in convergence and on all subsequent times an "evolve" message is received until a new target is set.

## Examples

See the [examples](examples) directory for Max patches that illustrate how the geneseq object works and object-oriented Python code that may be a little bit easier to read than the C.
