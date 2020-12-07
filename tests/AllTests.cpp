/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllTests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 13:38:59 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 13:43:04 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CppUTest/CommandLineTestRunner.h"

int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}
