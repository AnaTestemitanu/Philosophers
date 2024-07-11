/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_validation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antestem <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:55:58 by antestem          #+#    #+#             */
/*   Updated: 2024/07/11 16:56:00 by antestem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

/* contains_only_digits:
 *   Checks if a string contains only digits 0 - 9.
 *   Returns true if the string only contains digits.
 *   Returns false if the string contains a character 
 *   that is not a digit.
 *   
 *   Parameters:
 *     - str: The string to be checked.
 *   
 *   Returns:
 *     - A boolean indicating whether the string contains 
 *       only digits.
 */
static bool	contains_only_digits(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (false);
		i++;
	}
	return (true);
}

/* parse_integer:
 *   Converts a digit-only string into a positive integer.
 *   Returns the converted number between 0 and INT_MAX.
 *   Returns -1 if the converted number exceeds INT_MAX.
 *   
 *   Parameters:
 *     - str: The string to be converted.
 *   
 *   Returns:
 *     - The converted integer value or -1 if it exceeds INT_MAX.
 */
int	parse_integer(char *str)
{
	unsigned long long int	nb;
	int						i;

	i = 0;
	nb = 0;
	while (str[i] && (str[i] >= '0' && str[i] <= '9'))
	{
		nb = nb * 10 + (str[i] - '0');
		i++;
	}
	if (nb > INT_MAX)
		return (-1);
	return ((int)nb);
}

/* is_valid_input:
 *   Checks if all required arguments are valid, i.e. is a string of
 *   digits only, which does not exceed INT_MAX. Also checks if the 
 *   number of philosophers is valid (between 1 and MAX_PHILOSOPHERS).
 *   Returns true if all arguments are valid, false if one of them 
 *   is invalid.
 *   
 *   Parameters:
 *     - argc: The number of arguments.
 *     - argv: The array of argument strings.
 *   
 *   Returns:
 *     - A boolean indicating whether all inputs are valid.
 */
bool	is_valid_input(int argc, char **argv)
{
	int	i;
	int	nb;

	i = 1;
	while (i < argc)
	{
		if (!contains_only_digits(argv[i]))
			return (print_message(ERROR_INVALID_INPUT_DIGIT, argv[i], false));
		nb = parse_integer(argv[i]);
		if (i == 1 && (nb <= 0 || nb > MAX_PHILOSOPHERS))
			return (print_message(ERROR_INVALID_INPUT_RANGE,
					STR_MAX_PHILOSOPHERS, false));
		if (i != 1 && nb == -1)
			return (print_message(ERROR_INVALID_INPUT_DIGIT, argv[i], false));
		i++;
	}
	return (true);
}
