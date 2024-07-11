/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antestem <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:13:49 by antestem          #+#    #+#             */
/*   Updated: 2024/07/11 16:42:18 by antestem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <limits.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

/* Macros */

# define MAX_PHILOSOPHERS 250
# define STR_MAX_PHILOSOPHERS "250"

# ifndef DEBUG_FORMATTING
#  define DEBUG_FORMATTING 0
# endif

# define COLOR_RESET "\e[0m"
# define COLOR_RED "\e[31m"
# define COLOR_GREEN "\e[32m"
# define COLOR_PURPLE "\e[35m"
# define COLOR_CYAN "\e[36m"

# define PROGRAM_NAME "philosophers:"
# define USAGE_MESSAGE \
	"%s usage: ./philo <number_of_philosophers> \
<time_to_die> <time_to_eat> <time_to_sleep> \
[number_of_times_each_philosopher_must_eat]\n"
# define ERROR_INVALID_INPUT_DIGIT \
	"%s invalid input: %s: \
not a valid unsigned integer between 0 and 2147483647.\n"
# define ERROR_INVALID_INPUT_RANGE \
	"%s invalid input: \
there must be between 1 and %s philosophers.\n"
# define ERROR_THREAD_CREATION "%s error: Could not create thread.\n"
# define ERROR_MEMORY_ALLOCATION "%s error: Could not allocate memory.\n"
# define ERROR_MUTEX_CREATION "%s error: Could not create mutex.\n"

/* Structures */

typedef struct s_philosopher	t_philosopher;

typedef struct s_dining_table
{
	int							must_eat_count;
	time_t						start_time;
	time_t						time_to_die;
	time_t						time_to_eat;
	time_t						time_to_sleep;
	unsigned int				num_philosophers;
	pthread_t					grim_reaper_thread;
	bool						simulation_stopped;
	pthread_mutex_t				simulation_stop_lock;
	pthread_mutex_t				write_lock;
	pthread_mutex_t				*fork_locks;
	t_philosopher				**philosophers;
}								t_dining_table;

typedef struct s_philosopher
{
	pthread_t					thread;
	unsigned int				id;
	unsigned int				times_ate;
	unsigned int				fork[2];
	pthread_mutex_t				last_meal_lock;
	time_t						last_meal_time;
	t_dining_table				*dining_table;
}								t_philosopher;

typedef enum e_philosopher_status
{
	PHILO_DIED = 0,
	PHILO_EATING = 1,
	PHILO_SLEEPING = 2,
	PHILO_THINKING = 3,
	PHILO_GOT_FORK_1 = 4,
	PHILO_GOT_FORK_2 = 5
}								t_philosopher_status;

/* Function Prototypes */

/* table_initialization.c */
t_dining_table			*init_dining_table(int argc, char **argv,
							int index);

/* input_validation.c */
bool					is_valid_input(int argc, char **argv);
int						parse_integer(char *str);

/* philosopher_routines.c */
void					*philosopher_routine(void *data);

/* time_management.c */
time_t					get_current_time_in_ms(void);
void					philosopher_sleep(t_dining_table *dining_table,
							time_t sleep_duration);
void					delay_simulation_start(time_t start_time);

/* output.c */
void					philo_stat(t_philosopher *philosopher,
							bool is_reaper,
							t_philosopher_status status);
void					print_simulation_outcome(t_dining_table *dining_table);

/* grim_reaper.c */
void					*grim_reaper_routine(void *data);
bool					is_simulation_stopped(t_dining_table *dining_table);

/* cleanup.c */
int						print_error_and_exit(char *message,
							char *details,
							t_dining_table *dining_table);
void					*free_dining_table(t_dining_table *dining_table);
void					destroy_all_mutexes(t_dining_table *dining_table);
int						print_message(char *message, char *detail,
							int exit_code);
void					*print_error_and_return_null(char *message,
							char *details,
							t_dining_table *dining_table);

#endif
