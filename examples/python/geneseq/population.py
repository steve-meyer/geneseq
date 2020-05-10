import random
import math
import copy

from collections import defaultdict
from .individual import Individual


class Population(object):
    '''
    Class representing a collection of individuals
    '''
    def __init__(self, nucleotides='012345678', target='10300050', size=128):
        self.nucleotides = nucleotides
        self.target = target
        self.size = size
        self.individuals = defaultdict(list)
        self.count = 0
        self.generation = 1

    def evolve(self):
        '''
        Move the population ahead one generation by evolving it towards the target sequence
        '''
        new_generation = defaultdict(list)
        population_count = 0

        # 10% of fittest population automatically goes to the next generation
        s = int( math.ceil((10*self.count)/100) )
        scores = list(self.candidate_scores())
        top_candidates = []
        while s > 0:
            if len(top_candidates) == 0:
                next_top_score = scores.pop(0)
                top_candidates = copy.copy(self.individuals_with_score(next_top_score))
            new_gen_indiv = top_candidates.pop(random.randrange(len(top_candidates)))
            new_generation[new_gen_indiv.fitness].append(new_gen_indiv)
            s -= 1
            population_count += 1

        # The fittest 50% of the population will mate to produce offspring
        indivs = self.sorted_individiduals()
        if int((90*len(indivs))/100) + population_count < self.size:
            s = len(indivs) * 2
        elif int((90*len(indivs))/100) + population_count > self.size:
            s = self.size - population_count
        else:
            s = int( (90*self.size) / 100 )

        for _ in range(s):
            # Ensure no individual mates with itself
            end_index = 2 if len(indivs) < 4 else int(len(indivs)/2)
            parent1, parent2 = random.sample(indivs[:end_index], 2)

            child = parent1.mate(parent2)
            new_generation[child.fitness].append(child)
            population_count += 1

        self.individuals = new_generation
        self.count = population_count
        self.generation += 1

    def has_converged(self):
        '''
        The population has converged when at least one of its individuals matches the target
        '''
        return self.best_candidate().fitness == 0

    def seed(self, initial_population=2):
        '''
        When the population is empty, seed it based on the population params passed when initialized
        '''
        for _ in range(initial_population):
            chromosome = self.create_chromosome()
            self.add(Individual(self, chromosome))
        self.count += initial_population

    def create_chromosome(self):
        '''
        Create chromosome string out of the population's nucleotides
        '''
        return [random.choice(self.nucleotides) for _ in range(len(self.target))]

    def sorted_individiduals(self):
        flattened_indivs = [indiv for score_group in self.individuals.values() for indiv in score_group]
        flattened_indivs.sort(key=lambda x: x.fitness)
        return flattened_indivs

    def add(self, individual):
        self.individuals[individual.fitness].append(individual)

    def best_candidate(self):
        return random.choice(self.best_candidates())

    def best_candidates(self):
        # Get a list of all candidates with the lowest score
        return self.individuals[self.best_candidate_score()]

    def best_candidate_score(self):
        return self.candidate_scores()[0]

    def candidate_scores(self):
        return sorted(self.individuals.keys())

    def individuals_with_score(self, score):
        return self.individuals[score]

    def print_evolution_cycle(self):
        best_candidate = self.best_candidate()
        format_args = (self.generation, self.count, best_candidate, best_candidate.fitness)
        print("Generation: %i (%i)\tBest Chromosome: %s\tFitness: %i" % format_args)
