#include "philosophers.h"

/* eat_and_sleep_routine:
*   When a philosopher is ready to eat, he will wait for his fork mutexes to
*   be unlocked before locking them. Then the philosopher will eat for a certain
*   amount of time. The time of the last meal is recorded at the beginning of
*   the meal, not at the end, as per the subject's requirements.
*/
static void eat_and_sleep_routine(t_philosopher *philosopher)
{
    pthread_mutex_lock(&philosopher->dining_table->fork_locks[philosopher->fork[0]]);
    print_philosopher_status(philosopher, false, PHILO_GOT_FORK_1);
    pthread_mutex_lock(&philosopher->dining_table->fork_locks[philosopher->fork[1]]);
    print_philosopher_status(philosopher, false, PHILO_GOT_FORK_2);
    print_philosopher_status(philosopher, false, PHILO_EATING);
    pthread_mutex_lock(&philosopher->last_meal_lock);
    philosopher->last_meal_time = get_current_time_in_ms();
    pthread_mutex_unlock(&philosopher->last_meal_lock);
    philosopher_sleep(philosopher->dining_table, philosopher->dining_table->time_to_eat);
    if (!is_simulation_stopped(philosopher->dining_table))
    {
        pthread_mutex_lock(&philosopher->last_meal_lock);
        philosopher->times_ate += 1;
        pthread_mutex_unlock(&philosopher->last_meal_lock);
    }
    print_philosopher_status(philosopher, false, PHILO_SLEEPING);
    pthread_mutex_unlock(&philosopher->dining_table->fork_locks[philosopher->fork[1]]);
    pthread_mutex_unlock(&philosopher->dining_table->fork_locks[philosopher->fork[0]]);
    philosopher_sleep(philosopher->dining_table, philosopher->dining_table->time_to_sleep);
}

/* think_routine:
*   Once a philosopher is done sleeping, he will think for a certain
*   amount of time before starting to eat again.
*   The time_to_think is calculated depending on how long it has been
*   since the philosopher's last meal, the time_to_eat and the time_to_die
*   to determine when the philosopher will be hungry again.
*   This helps stagger philosopher's eating routines to avoid forks being
*   needlessly monopolized by one philosopher to the detriment of others.
*/
static void think_routine(t_philosopher *philosopher, bool silent)
{
    time_t time_to_think;

    pthread_mutex_lock(&philosopher->last_meal_lock);
    time_to_think = (philosopher->dining_table->time_to_die
            - (get_current_time_in_ms() - philosopher->last_meal_time)
            - philosopher->dining_table->time_to_eat) / 2;
    pthread_mutex_unlock(&philosopher->last_meal_lock);
    if (time_to_think < 0)
        time_to_think = 0;
    if (time_to_think == 0 && silent == true)
        time_to_think = 1;
    if (time_to_think > 600)
        time_to_think = 200;
    if (!silent)
        print_philosopher_status(philosopher, false, PHILO_THINKING);
    philosopher_sleep(philosopher->dining_table, time_to_think);
}

/* lone_philosopher_routine:
*   This routine is invoked when there is only a single philosopher.
*   A single philosopher only has one fork, and so cannot eat. The
*   philosopher will pick up that fork, wait as long as time_to_die and die.
*   This is a separate routine to make sure that the thread does not get
*   stuck waiting for the second fork in the eat routine.
*/
static void *lone_philosopher_routine(t_philosopher *philosopher)
{
    pthread_mutex_lock(&philosopher->dining_table->fork_locks[philosopher->fork[0]]);
    print_philosopher_status(philosopher, false, PHILO_GOT_FORK_1);
    philosopher_sleep(philosopher->dining_table, philosopher->dining_table->time_to_die);
    print_philosopher_status(philosopher, false, PHILO_DIED);
    pthread_mutex_unlock(&philosopher->dining_table->fork_locks[philosopher->fork[0]]);
    return (NULL);
}

/* philosopher_routine:
*   The philosopher thread routine. The philosopher must eat, sleep
*   and think. In order to avoid conflicts between philosopher threads,
*   philosophers with an even id start by thinking, which delays their
*   meal time by a small margin. This allows odd-id philosophers to
*   grab their forks first, avoiding deadlocks.
*/
void *philosopher_routine(void *data)
{
    t_philosopher *philosopher;

    philosopher = (t_philosopher *)data;
    if (philosopher->dining_table->must_eat_count == 0)
        return (NULL);
    pthread_mutex_lock(&philosopher->last_meal_lock);
    philosopher->last_meal_time = philosopher->dining_table->start_time;
    pthread_mutex_unlock(&philosopher->last_meal_lock);
    delay_simulation_start(philosopher->dining_table->start_time);
    if (philosopher->dining_table->time_to_die == 0)
        return (NULL);
    if (philosopher->dining_table->num_philosophers == 1)
        return (lone_philosopher_routine(philosopher));
    else if (philosopher->id % 2)
        think_routine(philosopher, true);
    while (!is_simulation_stopped(philosopher->dining_table))
    {
        eat_and_sleep_routine(philosopher);
        think_routine(philosopher, false);
    }
    return (NULL);
}
