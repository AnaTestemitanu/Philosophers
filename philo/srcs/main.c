/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antestem <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:56:46 by antestem          #+#    #+#             */
/*   Updated: 2024/07/11 16:56:47 by antestem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

/* start_simulation:
 *   Launches the simulation by creating a grim reaper thread as well as
 *   one thread for each philosopher.
 *   
 *   Returns true if the simulation was successfully started, 
 *   false if there was an error.
 *   
 *   Parameters:
 *     - dining_table: Pointer to the dining_table structure containing
 *       the philosophers and threads information.
 *   
 *   This function sets the start time for the simulation, then creates a 
 *   thread for each philosopher. If the number of philosophers is greater 
 *   than one, it also creates a grim reaper thread to monitor the simulation.
 *   If any thread creation fails, it prints an error message and exits.
 */
static bool	start_simulation(t_dining_table *dining_table)
{
	unsigned int	i;

	dining_table->start_time = get_current_time_in_ms()
		+ (dining_table->num_philosophers * 2 * 10);
	i = 0;
	while (i < dining_table->num_philosophers)
	{
		if (pthread_create(&dining_table->philosophers[i]->thread, NULL,
				&philosopher_routine, dining_table->philosophers[i]) != 0)
			return (print_error_and_exit(ERROR_THREAD_CREATION, NULL,
					dining_table));
		i++;
	}
	if (dining_table->num_philosophers > 1)
	{
		if (pthread_create(&dining_table->grim_reaper_thread, NULL,
				&grim_reaper_routine, dining_table) != 0)
			return (print_error_and_exit(ERROR_THREAD_CREATION, NULL,
					dining_table));
	}
	return (true);
}

/* stop_simulation:
 *   Waits for all threads to be joined then destroys mutexes and frees
 *   allocated memory.
 *   
 *   Parameters:
 *     - dining_table: Pointer to the dining_table structure containing
 *       the philosophers and threads information.
 *   
 *   This function waits for each philosopher thread to finish by calling 
 *   pthread_join. If there is a grim reaper thread, it waits for it to finish 
 *   as well. After all threads have been joined, it destroys all mutexes and 
 *   frees the allocated memory.
 */
static void	stop_simulation(t_dining_table *dining_table)
{
	unsigned int	i;

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

/* main:
 *   Validates input arguments, initializes 
 *   the dining table, starts the simulation, and stops the simulation 
 *   once it finishes.
 *   
 *   Parameters:
 *     - argc: The number of command-line arguments.
 *     - argv: The array of command-line arguments.
 *   
 *   Returns:
 *     - EXIT_SUCCESS if the program completes successfully.
 *     - EXIT_FAILURE if there is an error during initialization 
 *       or simulation.
 *   
 *   This function first checks if the number of arguments is valid. Then it 
 *   validates each argument and initializes the dining table. If everything 
 *   is set up correctly, it starts the simulation and stops it once it 
 *   finishes. If there is any error during these steps, it prints an error 
 *   message and exits with a failure status.
 */
int	main(int argc, char **argv)
{
	t_dining_table	*dining_table;

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
