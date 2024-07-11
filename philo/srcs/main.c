#include "philosophers.h"

/* start_simulation:
*   Launches the simulation by creating a grim reaper thread as well as
*   one thread for each philosopher.
*   Returns true if the simulation was successfully started, false if there
*   was an error. 
*/
static bool start_simulation(t_dining_table *dining_table)
{
    unsigned int i;

    dining_table->start_time = get_current_time_in_ms() + (dining_table->num_philosophers * 2 * 10);
    i = 0;
    while (i < dining_table->num_philosophers)
    {
        if (pthread_create(&dining_table->philosophers[i]->thread, NULL,
                           &philosopher_routine, dining_table->philosophers[i]) != 0)
            return (print_error_and_exit(ERROR_THREAD_CREATION, NULL, dining_table));
        i++;
    }
    if (dining_table->num_philosophers > 1)
    {
        if (pthread_create(&dining_table->grim_reaper_thread, NULL,
                           &grim_reaper_routine, dining_table) != 0)
            return (print_error_and_exit(ERROR_THREAD_CREATION, NULL, dining_table));
    }
    return (true);
}

/* stop_simulation:
*   Waits for all threads to be joined then destroys mutexes and frees
*   allocated memory.
*/
static void stop_simulation(t_dining_table *dining_table)
{
    unsigned int i;

    i = 0;
    while (i < dining_table->num_philosophers)
    {
        pthread_join(dining_table->philosophers[i]->thread, NULL);
        i++;
    }
    if (dining_table->num_philosophers > 1)
        pthread_join(dining_table->grim_reaper_thread, NULL);
    if (DEBUG_FORMATTING == true && dining_table->must_eat_count != -1)
        print_simulation_outcome(dining_table);
    destroy_all_mutexes(dining_table);
    free_dining_table(dining_table);
}

int main(int argc, char **argv)
{
    t_dining_table *dining_table;

    dining_table = NULL;
    if (argc - 1 < 4 || argc - 1 > 5)
        return (print_message(USAGE_MESSAGE, NULL, EXIT_FAILURE));
    if (!is_valid_input(argc, argv))
        return (EXIT_FAILURE);
    dining_table = init_dining_table(argc, argv, 1);
    if (!dining_table)
        return (EXIT_FAILURE);
    if (!start_simulation(dining_table))
        return (EXIT_FAILURE);
    stop_simulation(dining_table);
    return (EXIT_SUCCESS);
}
