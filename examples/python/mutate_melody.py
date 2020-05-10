import random
from geneseq.individual import Individual
from geneseq.population import Population


# Number of individuals in each generation
POPULATION_SIZE = 64

# Valid gene nucleotides
GENES = '012345678'

# Target string to be generated
TARGET = "12030050"


def main():
    population = Population(GENES, TARGET, POPULATION_SIZE)
    population.seed()

    while not population.has_converged():
        population.print_evolution_cycle()
        population.evolve()

    population.print_evolution_cycle()


if __name__ == '__main__':
    main()
