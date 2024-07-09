#include "philosophers.h"

/* print_status_debug:
*   Prints the philosopher's status in an easier to read,
*   colorful format to help with debugging. For fork-taking
*   statuses, extra information is displayed to show which fork
*   the philosopher has taken.
*/
static void print_status_debug(t_philosopher *philosopher, char *color,
                               char *status_str, t_philosopher_status status)
{
    if (status == PHILO_GOT_FORK_1)
        printf("[%10ld]\t%s%03d\t%s\e[0m: fork [%d]\n",
               get_current_time_in_ms() - philosopher->dining_table->start_time,
               color, philosopher->id + 1, status_str, philosopher->fork[0]);
    else if (status == PHILO_GOT_FORK_2)
        printf("[%10ld]\t%s%03d\t%s\e[0m: fork [%d]\n",
               get_current_time_in_ms() - philosopher->dining_table->start_time,
               color, philosopher->id + 1, status_str, philosopher->fork[1]);
    else
        printf("[%10ld]\t%s%03d\t%s\e[0m\n",
               get_current_time_in_ms() - philosopher->dining_table->start_time,
               color, philosopher->id + 1, status_str);
}

/* write_status_debug:
*   Redirects the status writing for debug mode. For this option,
*   the DEBUG_FORMATTING option must be set to 1 in philosophers.h.
*/
static void write_status_debug(t_philosopher *philosopher, t_philosopher_status status)
{
    if (status == PHILO_DIED)
        print_status_debug(philosopher, COLOR_RED, "died", status);
    else if (status == PHILO_EATING)
        print_status_debug(philosopher, COLOR_GREEN, "is eating", status);
    else if (status == PHILO_SLEEPING)
        print_status_debug(philosopher, COLOR_CYAN, "is sleeping", status);
    else if (status == PHILO_THINKING)
        print_status_debug(philosopher, COLOR_CYAN, "is thinking", status);
    else if (status == PHILO_GOT_FORK_1)
        print_status_debug(philosopher, COLOR_PURPLE, "has taken a fork", status);
    else if (status == PHILO_GOT_FORK_2)
        print_status_debug(philosopher, COLOR_PURPLE, "has taken a fork", status);
}

/* print_status:
*   Prints a philosopher's status in plain text as required by the project
*   subject:
*       timestamp_in_ms X status
*/
static void print_status(t_philosopher *philosopher, char *status_str)
{
    printf("%ld %d %s\n", get_current_time_in_ms() - philosopher->dining_table->start_time,
           philosopher->id + 1, status_str);
}

/* print_philosopher_status:
*   Prints the status of a philosopher as long as the simulation is
*   still active. Locks the write mutex to avoid intertwined messages
*   from different threads.
*
*   If DEBUG_FORMATTING is set to 1 in philosophers.h, the status will
*   be formatted with colors and extra information to help with debugging.
*   Otherwise the output will be the regular format required by the project
*   subject.
*/
void print_philosopher_status(t_philosopher *philosopher, bool is_reaper, t_philosopher_status status)
{
    pthread_mutex_lock(&philosopher->dining_table->write_lock);
    if (is_simulation_stopped(philosopher->dining_table) && !is_reaper)
    {
        pthread_mutex_unlock(&philosopher->dining_table->write_lock);
        return;
    }
    if (DEBUG_FORMATTING)
    {
        write_status_debug(philosopher, status);
        pthread_mutex_unlock(&philosopher->dining_table->write_lock);
        return;
    }
    if (status == PHILO_DIED)
        print_status(philosopher, "died");
    else if (status == PHILO_EATING)
        print_status(philosopher, "is eating");
    else if (status == PHILO_SLEEPING)
        print_status(philosopher, "is sleeping");
    else if (status == PHILO_THINKING)
        print_status(philosopher, "is thinking");
    else if (status == PHILO_GOT_FORK_1 || status == PHILO_GOT_FORK_2)
        print_status(philosopher, "has taken a fork");
    pthread_mutex_unlock(&philosopher->dining_table->write_lock);
}

/* print_simulation_outcome:
*   Prints the outcome of the simulation if a number of times to
*   eat was specified. Only used for debug purposes.
*/
void print_simulation_outcome(t_dining_table *dining_table)
{
    unsigned int i;
    unsigned int full_count;

    full_count = 0;
    i = 0;
    while (i < dining_table->num_philosophers)
    {
        if (dining_table->philosophers[i]->times_ate >= (unsigned int)dining_table->must_eat_count)
            full_count++;
        i++;
    }
    pthread_mutex_lock(&dining_table->write_lock);
    printf("%d/%d philosophers had at least %d meals.\n",
           full_count, dining_table->num_philosophers, dining_table->must_eat_count);
    pthread_mutex_unlock(&dining_table->write_lock);
    return;
}