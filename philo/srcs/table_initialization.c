#include "philosophers.h"

/* init_fork_mutexes:
*   Allocates memory and initializes fork mutexes.
*   Returns a pointer to the fork mutex array, or NULL if an error occurred. 
*/
static pthread_mutex_t *init_fork_mutexes(t_dining_table *dining_table)
{
    pthread_mutex_t *forks;
    unsigned int i;

    forks = malloc(sizeof(pthread_mutex_t) * dining_table->num_philosophers);
    if (!forks)
        return (print_error_and_return_null(ERROR_MEMORY_ALLOCATION, NULL, dining_table));
    i = 0;
    while (i < dining_table->num_philosophers)
    {
        if (pthread_mutex_init(&forks[i], NULL) != 0)
            return (print_error_and_return_null(ERROR_MUTEX_CREATION, NULL, dining_table));
        i++;
    }
    return (forks);
}

/* assign_forks_to_philosopher:
*   Assigns two fork ids to each philosopher. Even-numbered philosophers
*   get their fork order switched. This is because the order in which
*   philosophers take their forks matters.
*
*   For example with 3 philosophers:
*       Philosopher #1 (id: 0) will want fork 0 and fork 1
*       Philosopher #2 (id: 1) will want fork 1 and fork 2
*       Philosopher #3 (id: 2) will want fork 2 and fork 0
*   If philosopher #1 takes fork 0, philosopher #2 takes fork 1 and philosopher #3 takes fork 2,
*   there is a deadlock. Each will be waiting for their second fork which is
*   in use by another philosopher.
*
*   Making even id philosophers "left-handed" helps:
*       Philosopher #1 (id: 0) takes fork 1 and then fork 0
*       Philosopher #2 (id: 1) takes fork 1 and then fork 2
*       Philosopher #3 (id: 2) takes fork 0 and then fork 2
*   Now, philosopher #1 takes fork 1, philosopher #3 takes fork 0 and philosopher #2 waits patiently.
*   Fork 2 is free for philosopher #3 to take, so he eats. When he is done philosopher #1 can
*   take fork 0 and eat. When he is done, philosopher #2 can finally get fork 1 and eat.
*/
static void assign_forks_to_philosopher(t_philosopher *philosopher)
{
    philosopher->fork[0] = philosopher->id;
    philosopher->fork[1] = (philosopher->id + 1) % philosopher->dining_table->num_philosophers;
    if (philosopher->id % 2)
    {
        philosopher->fork[0] = (philosopher->id + 1) % philosopher->dining_table->num_philosophers;
        philosopher->fork[1] = philosopher->id;
    }
}

/* init_philosophers:
*   Allocates memory for each philosopher and initializes their values.
*   Returns a pointer to the array of philosophers or NULL if
*   initialization failed.
*/
static t_philosopher **init_philosophers(t_dining_table *dining_table)
{
    t_philosopher **philosophers;
    unsigned int i;

    philosophers = malloc(sizeof(t_philosopher) * dining_table->num_philosophers);
    if (!philosophers)
        return (print_error_and_return_null(ERROR_MEMORY_ALLOCATION, NULL, dining_table));
    i = 0;
    while (i < dining_table->num_philosophers)
    {
        philosophers[i] = malloc(sizeof(t_philosopher));
        if (!philosophers[i])
            return (print_error_and_return_null(ERROR_MEMORY_ALLOCATION, NULL, dining_table));
        if (pthread_mutex_init(&philosophers[i]->last_meal_lock, NULL) != 0)
            return (print_error_and_return_null(ERROR_MUTEX_CREATION, NULL, dining_table));
        philosophers[i]->dining_table = dining_table;
        philosophers[i]->id = i;
        philosophers[i]->times_ate = 0;
        assign_forks_to_philosopher(philosophers[i]);
        i++;
    }
    return (philosophers);
}

/* init_global_mutexes:
*   Initializes mutex locks for forks, writing and the stop simulation
*   flag.
*   Returns true if the initializations were successful, false if
*   initialization failed.
*/
static bool init_global_mutexes(t_dining_table *dining_table)
{
    dining_table->fork_locks = init_fork_mutexes(dining_table);
    if (!dining_table->fork_locks)
        return (false);
    if (pthread_mutex_init(&dining_table->simulation_stop_lock, NULL) != 0)
        return (print_error_and_exit(ERROR_MUTEX_CREATION, NULL, dining_table));
    if (pthread_mutex_init(&dining_table->write_lock, NULL) != 0)
        return (print_error_and_exit(ERROR_MUTEX_CREATION, NULL, dining_table));
    return (true);
}

/* init_dining_table:
*   Initializes the "dining table", the data structure containing
*   all of the program's parameters.
*   Returns a pointer to the allocated table structure, or NULL if
*   an error occurred during initialization.
*/
t_dining_table *init_dining_table(int argc, char **argv, int i)
{
    t_dining_table *dining_table;

    dining_table = malloc(sizeof(t_dining_table));
    if (!dining_table)
        return (print_error_and_return_null(ERROR_MEMORY_ALLOCATION, NULL, NULL));
    dining_table->num_philosophers = parse_integer(argv[i++]);
    dining_table->time_to_die = parse_integer(argv[i++]);
    dining_table->time_to_eat = parse_integer(argv[i++]);
    dining_table->time_to_sleep = parse_integer(argv[i++]);
    dining_table->must_eat_count = -1;
    if (argc - 1 == 5)
        dining_table->must_eat_count = parse_integer(argv[i]);
    dining_table->philosophers = init_philosophers(dining_table);
    if (!dining_table->philosophers)
        return (NULL);
    if (!init_global_mutexes(dining_table))
        return (NULL);
    dining_table->simulation_stopped = false;
    return (dining_table);
}
