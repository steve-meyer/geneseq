import random


class Individual(object):
    '''
    Class representing individual in population. A single chromosome individual.
    '''
    def __init__(self, population, chromosome):
        self.population = population
        self.chromosome = chromosome
        self.fitness = self.cal_fitness()

    def __str__(self):
        return "".join(self.chromosome)

    def mate(self, par2):
        '''
        Perform mating and produce new offspring
        '''

        # chromosome for offspring
        child_chromosome = []
        for gp1, gp2 in zip(self.chromosome, par2.chromosome):

            # random probability
            prob = random.random()

            # if prob is less than 0.45, insert gene
            # from parent 1
            if prob < 0.45:
                child_chromosome.append(gp1)

            # if prob is between 0.45 and 0.90, insert
            # gene from parent 2
            elif prob < 0.90:
                child_chromosome.append(gp2)

            # otherwise insert random gene(mutate),
            # for maintaining diversity
            else:
                child_chromosome.append(random.choice(self.population.nucleotides))

        # create new Individual(offspring) using
        # generated chromosome for offspring
        return Individual(self.population, child_chromosome)

    def cal_fitness(self):
        '''
        Calculate fittness score, it is the number of
        characters in string which differ from target
        string.
        '''
        fitness = 0
        for gs, gt in zip(self.chromosome, self.population.target):
            if gs != gt: fitness+= 1
        return fitness
