/**
 geneseq - a genetic algorithm melody generator/sequencer for Max/MSP
 steve meyer
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object


#define POPULATION_SIZE 64 // number of individuals that will improvise/evolve toward the target melody
#define SEQ_STEPS 8        // sequence size TODO: make this configurable
#define GENES 8            // sequences will be based on numbers 0-8. 0 is silence,
                         // 1-8 are scale steps tonic through octave (i.e., 5 = 5th)
#define PARENTS 2          // number of individuals that provide genes from to new children
#define INIT_POP 4         // starter population TODO: not fully implemented


/*
 An individual improviser.

 An individual is a member of the geneseq population. The first generation's patterns
 will be generated randomly from the valid genes. Successive generations are bred
 from previous generations.

 All improvisors are scored. The fitness score is an indication of how well the improviser
 has adapted/resolved to the target melody after each evolution/breeding. The random score is
 "in the right place at the right time." Did the improviser luck out and get the right gig?
 */
struct individual {
    t_atom pattern[SEQ_STEPS];
    int fitness_score;
    double random_score;
};
int default_target[] = {1,2,0,3,0,0,5,0};
int parent_indices[] = {0, 1};


////////////////////////// object struct
typedef struct _geneseq
{
    t_object  ob;     // the object itself (must be first)
    void *m_outlet5;  // bang on convergence
    void *m_outlet4;  // the target pattern
    void *m_outlet3;  // the current generation
    void *m_outlet2;  // the best candidate score
    void *m_outlet1;  // the best candidate pattern
    t_atom target[SEQ_STEPS];
    struct individual population[POPULATION_SIZE];
    int generation;
    int current_population;
} t_geneseq;

///////////////////////// function prototypes
// Max standard functions
void *geneseq_new(t_symbol *s, long argc, t_atom *argv);
void geneseq_free(t_geneseq *x);
void geneseq_assist(t_geneseq *x, void *b, long m, long a, char *s);

// Max object functions
void geneseq_bang(t_geneseq *x);
void geneseq_target(t_geneseq *x, t_symbol *s, long argc, t_atom *argv);
void geneseq_gettarget(t_geneseq *x, t_symbol *s, long argc, t_atom *argv);
void geneseq_evolve(t_geneseq *x, t_symbol *s, long argc, t_atom *argv);
void seed(t_geneseq *x);
void breed(t_geneseq *x, int breeding_pop, int child_idx);

// Internal utilities
void unique_sample(int numbers[], int max, int k);
int compare_individuals(const void *a, const void *b);

//////////////////////// global class pointer variable
void *geneseq_class;


void ext_main(void *r)
{
	t_class *c;

	c = class_new("geneseq", (method)geneseq_new, (method)geneseq_free, (long)sizeof(t_geneseq),
				  0L /* leave NULL!! */, A_GIMME, 0);

	class_addmethod(c, (method)geneseq_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)geneseq_bang, "bang", 0);
    class_addmethod(c, (method)geneseq_target, "target", A_GIMME, 0);
    class_addmethod(c, (method)geneseq_gettarget, "gettarget", A_GIMME, 0);
    class_addmethod(c, (method)geneseq_evolve, "evolve", A_GIMME, 0);

	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	geneseq_class = c;

	post("I am the geneseq object");
}


void geneseq_bang(t_geneseq *x)
{
    outlet_int(x->m_outlet3, x->generation);
    outlet_int(x->m_outlet2, x->population[0].fitness_score);
    outlet_list(x->m_outlet1, NULL, SEQ_STEPS, x->population[0].pattern);
}


/*
 Evolve the population by one generation towards convergence with the target melody.

 The population is always in a sorted state. Identify the fittest 10% of the population
 to move to the next generation. Then use the fittest 50% of the population to breed
 new children to refill the remaining population. Finally resort the population and
 generate outputs: bang if converged and send out best candidate, etc.
 */
void geneseq_evolve(t_geneseq *x, t_symbol *s, long argc, t_atom *argv)
{
    if (x->population[0].fitness_score != 0)
    {
        x->generation += 1;
        int num_fittest = ceil((x->current_population * 10) / 100.0);
        for (int i = 0; i < num_fittest; i++)
            x->population[i].random_score = random() / (double) RAND_MAX;

        int breeding_pop = x->current_population / 2;
        int num_children = x->current_population * 2;
        int max = num_children;
        if (num_children + num_fittest > POPULATION_SIZE)
        {
            num_children = POPULATION_SIZE - num_fittest;
            max = POPULATION_SIZE;
        }

        x->current_population = num_fittest;
        for (int i = num_fittest; i < max; i++)
        {
            breed(x, breeding_pop, i);
            x->current_population += 1;
        }
    }
    qsort(x->population, POPULATION_SIZE, sizeof(struct individual), compare_individuals);

    if (x->population[0].fitness_score == 0) outlet_bang(x->m_outlet5);
    geneseq_bang(x);
}


/*
 Given a set of parents breed a new individual for the given child index.

 Breeding population is a count of the number of eligible parents to randomly choose from.
 Once two parent indices are chosen the child's new pattern is based on a random selection
 of parent 1's or parent 2's genes. 10% of the time the genes are the result of a
 mutation.
 */
void breed(t_geneseq *x, int breeding_pop, int child_idx)
{
    unique_sample(parent_indices, breeding_pop, PARENTS);
    struct individual parent1 = x->population[parent_indices[0]];
    struct individual parent2 = x->population[parent_indices[1]];
    struct individual *child = &x->population[child_idx];

    int score = 0;
    int step_val;
    for (int step = 0; step < SEQ_STEPS; step++)
    {
        // Randomly sample from the parents
        if (rand() % 2 == 0)
            step_val = atom_getlong(parent1.pattern+step);
        else
            step_val = atom_getlong(parent2.pattern+step);

        // About 10% of the time, mutations occur
        if (rand() % 100 > 90)
            step_val = random() % (GENES + 1);

        atom_setlong(child->pattern+step, step_val);
        if (step_val != atom_getlong(x->target+step))
            score += 1;
    }
    child->fitness_score = score;
    child->random_score = rand() / (double) RAND_MAX;
}


/*
 Update the target sequence to be used for the population. Will also regenerate/reseed the population.
 */
void geneseq_target(t_geneseq *x, t_symbol *s, long argc, t_atom *argv)
{
    if (argc != SEQ_STEPS)
    {
        error("sequence not %i steps", SEQ_STEPS);
    }
    else
    {
        for (int step = 0; step < SEQ_STEPS; step++)
            atom_setlong(x->target+step, atom_getlong(argv+step));

        seed(x);
    }
}


/*
 Generate an entirely new population of individuals.
 */
void seed(t_geneseq *x)
{
    int score, step_val;
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        score = 0;
        for (int step = 0; step < SEQ_STEPS; step++)
        {
            step_val = random() % (GENES + 1);
            atom_setlong(x->population[i].pattern+step, step_val);
            if (step_val != atom_getlong(x->target+step))
                score += 1;
        }
        x->population[i].fitness_score = score;
        x->population[i].random_score = random() / (double) RAND_MAX;
    }
    qsort(x->population, POPULATION_SIZE, sizeof(struct individual), compare_individuals);
}


void geneseq_gettarget(t_geneseq *x, t_symbol *s, long argc, t_atom *argv)
{
    outlet_list(x->m_outlet4, NULL, SEQ_STEPS, x->target);
}


void geneseq_assist(t_geneseq *x, void *b, long io, long index, char *s)
{
    switch(io)
    {
        case ASSIST_INLET:
            sprintf(s, "bang outputs best pattern, score, generation; 'evolve' breeds, then outputs pattern, score, generation; 'target' + int list sets the target sequence; 'gettarget' sends pattern's current target");
            break;
        case ASSIST_OUTLET:
            switch(index)
            {
                case 0:
                    sprintf(s, "outlet 1: best sequence pattern");
                    break;
                case 1:
                    sprintf(s, "outlet 2: best sequence fitness");
                    break;
                case 2:
                    sprintf(s, "outlet 3: current generation");
                    break;
                case 3:
                    sprintf(s, "outlet 4: list for target sequence when gettarget message received");
                    break;
                case 4:
                    sprintf(s, "outlet 5: bang on convergence");
                    break;
            }
    }
}


void geneseq_free(t_geneseq *x)
{
	sysmem_freeptr(x->population);
}


void *geneseq_new(t_symbol *s, long argc, t_atom *argv)
{
	t_geneseq *x = NULL;
    x = (t_geneseq *)object_alloc(geneseq_class);
    atom_setlong(x->generation, 0);
    x->current_population = INIT_POP;

    x->m_outlet5 = bangout((t_object *)x);
    x->m_outlet4 = outlet_new((t_object *)x, NULL);
    x->m_outlet3 = intout((t_object *)x);
    x->m_outlet2 = intout((t_object *)x);
    x->m_outlet1 = outlet_new((t_object *)x, NULL);

    for (int i = 0; i < SEQ_STEPS; i++)
        atom_setlong(x->target+i, default_target[i]);

    seed(x);

	return (x);
}


// qsort struct comparision function
int compare_individuals(const void *a, const void *b)
{
    struct individual *ia = (struct individual *)a;
    struct individual *ib = (struct individual *)b;
    double a_sort = ia->fitness_score + ia->random_score;
    double b_sort = ib->fitness_score + ib->random_score;
    return (int)(1000.f * a_sort - 1000.f * b_sort);
}


/*
 Insert k ints into numbers[] between 0 and max.
 Do not allow duplicates.
 */
void unique_sample(int numbers[], int max, int k)
{
    if (k > max) k = max;

    int sample_range[max];
    for (int i = 0; i < max; i++)
        sample_range[i] = i;

    int rand_index, tmp;
    for (int i = 0; i < k; i++)
    {
        rand_index = rand() % (max - i);
        numbers[i] = sample_range[rand_index];

        tmp = sample_range[max - 1];
        sample_range[max - 1] = numbers[i];
        sample_range[rand_index] = tmp;
    }
}
