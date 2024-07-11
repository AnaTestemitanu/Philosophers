/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antestem <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:57:04 by antestem          #+#    #+#             */
/*   Updated: 2024/07/11 16:57:06 by antestem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

/* free_dining_table:
*   Frees all of the memory allocated by the program.
*   Returns a NULL pointer if there is nothing to free,
*   or when all memory has been freed.
*   
*   Parameters:
*   - dining_table: Pointer to the dining table structure 
*     which contains all allocated resources.
*   
*   This function first checks if the dining_table is NULL.
*   If not, it frees the fork locks, then iterates over 
*   the philosophers array and frees each philosopher. 
*   Finally, it frees the dining_table itself.
*/
void	*free_dining_table(t_dining_table *dining_table)
{
	unsigned int	i;

	if (!dining_table)
		return (NULL);
	if (dining_table->fork_locks != NULL)
		free(dining_table->fork_locks);
	if (dining_table->philosophers != NULL)
	{
		i = 0;
		while (i < dining_table->num_philosophers)
		{
			if (dining_table->philosophers[i] != NULL)
				free(dining_table->philosophers[i]);
			i++;
		}
		free(dining_table->philosophers);
	}
	free(dining_table);
	return (NULL);
}

/* destroy_all_mutexes:
*   Destroys every mutex created by the program: fork locks, 
*   meal locks, the write lock, and the simulation stopper lock.
*   
*   Parameters:
*   - dining_table: Pointer to the dining table structure 
*     which contains the mutexes.
*   
*   This function iterates over all philosophers, destroying 
*   their fork locks and last meal locks. Then it destroys 
*   the write lock and the simulation stop lock.
*/
void	destroy_all_mutexes(t_dining_table *dining_table)
{
	unsigned int	i;

	i = 0;
	while (i < dining_table->num_philosophers)
	{
		pthread_mutex_destroy(&dining_table->fork_locks[i]);
		pthread_mutex_destroy(&dining_table->philosophers[i]->last_meal_lock);
		i++;
	}
	pthread_mutex_destroy(&dining_table->write_lock);
	pthread_mutex_destroy(&dining_table->simulation_stop_lock);
}

/* print_message:
*   Writes a message to the console. Returns the provided exit number.
*   Used for error management.
*   
*   Parameters:
*   - message: The format string for the message to be printed.
*   - detail: Additional detail to be included in the message.
*   - exit_code: The exit code to be returned.
*   
*   If detail is NULL, the message is printed with the program name.
*   Otherwise, the message is printed with the program name and detail.
*/
int	print_message(char *message, char *detail, int exit_code)
{
	if (!detail)
		printf(message, PROGRAM_NAME);
	else
		printf(message, PROGRAM_NAME, detail);
	return (exit_code);
}

/* print_error_and_exit:
*   Frees any allocated memory, prints an error message and
*   returns 0 to indicate failure.
*   Used for error management during initialization.
*   
*   Parameters:
*   - message: The format string for the error message.
*   - details: Additional detail to be included in the error message.
*   - dining_table: Pointer to the dining table structure to be freed.
*   
*   This function frees the dining table using free_dining_table
*   and prints an error message using print_message, then returns 0.
*/
int	print_error_and_exit(char *message, char *details, \
t_dining_table *dining_table)
{
	if (dining_table != NULL)
		free_dining_table(dining_table);
	return (print_message(message, details, 0));
}

/* print_error_and_return_null:
*   Frees any allocated memory, prints an error message 
*   and returns a NULL pointer.
*   Used for error management during initialization.
*   
*   Parameters:
*   - message: The format string for the error message.
*   - details: Additional detail to be included in the error message.
*   - dining_table: Pointer to the dining table structure to be freed.
*   
*   This function frees the dining table using free_dining_table
*   and prints an error message using print_message, then returns NULL.
*/
void	*print_error_and_return_null(char *message, \
char *details, t_dining_table *dining_table)
{
	if (dining_table != NULL)
		free_dining_table(dining_table);
	print_message(message, details, EXIT_FAILURE);
	return (NULL);
}
