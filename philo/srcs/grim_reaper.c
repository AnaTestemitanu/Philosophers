/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   grim_reaper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antestem <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:53:12 by antestem          #+#    #+#             */
/*   Updated: 2024/07/11 16:53:13 by antestem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

/* set_simulation_stop_flag:
 *   Sets the simulation stop flag to the given state (true or false).
 *   This flag is used to signal that the simulation should stop,
 *   for example, when a philosopher dies or all philosophers 
 *   have eaten enough. Only the grim reaper thread can set this 
 *   flag to ensure proper synchronization. The flag is protected 
 *   by a mutex to ensure thread safety.
 *   
 *   Parameters:
 *     - dining_table: Pointer to the dining_table structure 
 *       containing the flag.
 *     - state: Boolean value to set the simulation stop flag to 
 *       (true to stop, false to continue).
 */
static void	set_simulation_stop_flag(t_dining_table *dining_table, bool state)
{
	pthread_mutex_lock(&dining_table->simulation_stop_lock);
	dining_table->simulation_stopped = state;
	pthread_mutex_unlock(&dining_table->simulation_stop_lock);
}

/* is_simulation_stopped:
 *   Checks whether the simulation is currently stopped.
 *   This function reads the simulation stop flag, which is protected 
 *   by a mutex to allow safe access from multiple threads.
 *   
 *   Returns true if the simulation stop flag is set to true, 
 *   indicating the simulation should stop. Returns false if the 
 *   flag is set to false, indicating the simulation should continue.
 *   
 *   Parameters:
 *     - dining_table: Pointer to the dining_table structure 
 *       containing the flag.
 *   
 *   Returns:
 *     - A boolean indicating whether the simulation is stopped.
 */
bool	is_simulation_stopped(t_dining_table *dining_table)
{
	bool	stopped;

	stopped = false;
	pthread_mutex_lock(&dining_table->simulation_stop_lock);
	if (dining_table->simulation_stopped == true)
		stopped = true;
	pthread_mutex_unlock(&dining_table->simulation_stop_lock);
	return (stopped);
}

/* check_if_philosopher_should_die:
 *   Checks if a philosopher should be killed based on the 
 *   time since their last meal and the time_to_die parameter.
 *   If the time since the last meal exceeds the time_to_die, 
 *   the simulation stop flag is set, the philosopher's death is 
 *   recorded, and the function returns true. If the philosopher 
 *   should not die yet, the function returns false.
 *   
 *   Parameters:
 *     - philosopher: Pointer to the philosopher structure to check.
 *   
 *   Returns:
 *     - A boolean indicating whether the philosopher has died.
 */
static bool	check_if_philosopher_should_die(t_philosopher *philosopher)
{
	time_t	current_time;

	current_time = get_current_time_in_ms();
	if ((current_time - philosopher->last_meal_time) >= \
	philosopher->dining_table->time_to_die)
	{
		set_simulation_stop_flag(philosopher->dining_table, true);
		philo_stat(philosopher, true, PHILO_DIED);
		pthread_mutex_unlock(&philosopher->last_meal_lock);
		return (true);
	}
	return (false);
}

/* check_end_conditions:
 *   Checks each philosopher to see if one of two end conditions
 *   has been reached. Stops the simulation if a philosopher needs
 *   to be killed, or if every philosopher has eaten enough.
 *   
 *   Returns true if an end condition has been reached, false if not.
 *   
 *   Parameters:
 *     - dining_table: Pointer to the dining_table structure 
 *       containing the philosophers.
 *   
 *   Returns:
 *     - A boolean indicating whether an end condition has been reached.
 */
static bool	check_end_conditions(t_dining_table *dining_table)
{
	unsigned int	i;
	bool			all_philosophers_ate_enough;

	all_philosophers_ate_enough = true;
	i = 0;
	while (i < dining_table->num_philosophers)
	{
		pthread_mutex_lock(&dining_table->philosophers[i]->last_meal_lock);
		if (check_if_philosopher_should_die(dining_table->philosophers[i]))
			return (true);
		if (dining_table->must_eat_count != -1)
			if (dining_table->philosophers[i]->\
			times_ate < (unsigned int)dining_table->must_eat_count)
				all_philosophers_ate_enough = false;
		pthread_mutex_unlock(&dining_table->philosophers[i]->last_meal_lock);
		i++;
	}
	if (dining_table->must_eat_count != -1
		&& all_philosophers_ate_enough == true)
	{
		set_simulation_stop_flag(dining_table, true);
		return (true);
	}
	return (false);
}

/* grim_reaper_routine:
 *   The grim reaper thread's routine. Checks if a philosopher must
 *   be killed and if all philosophers ate enough. If one of those two
 *   end conditions are reached, it stops the simulation.
 *   
 *   Parameters:
 *     - data: Pointer to the dining_table structure containing 
 *       the simulation data.
 *   
 *   Returns:
 *     - A NULL pointer when the simulation stops.
 */
void	*grim_reaper_routine(void *data)
{
	t_dining_table	*dining_table;

	dining_table = (t_dining_table *)data;
	if (dining_table->must_eat_count == 0)
		return (NULL);
	set_simulation_stop_flag(dining_table, false);
	delay_simulation_start(dining_table->start_time);
	while (true)
	{
		if (check_end_conditions(dining_table) == true)
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
