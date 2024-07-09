#include "philosophers.h"

/* get_current_time_in_ms:
*   Gets the current time in milliseconds since the Epoch (1970-01-01 00:00:00).
*   Returns the time value.
*/
time_t get_current_time_in_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/* philosopher_sleep:
*   Pauses the philosopher thread for a certain amount of time in milliseconds.
*   Periodically checks to see if the simulation has ended during the sleep
*   time and cuts the sleep short if it has.
*/
void philosopher_sleep(t_dining_table *dining_table, time_t sleep_time)
{
    time_t wake_up_time;

    wake_up_time = get_current_time_in_ms() + sleep_time;
    while (get_current_time_in_ms() < wake_up_time)
    {
        if (is_simulation_stopped(dining_table))
            break;
        usleep(100);
    }
}

/* delay_simulation_start:
*   Waits for a small delay at the beginning of each thread's execution
*   so that all threads start at the same time with the same start time
*   reference. This ensures the grim reaper thread is synchronized with
*   the philosopher threads.
*/
void delay_simulation_start(time_t start_time)
{
    while (get_current_time_in_ms() < start_time)
        continue;
}
