/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 13:40:03 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 13:43:06 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CppuTest/TestHarness.h"

TEST_GROUP(alwaysTrue)
{
	bool		testValue;

	void		expect(bool value)
	{
		testValue = value;
	}
	void		given(void)
	{
		CHECK_EQUAL(true, testValue);
	}
};

TEST(alwaysTrue, expectTrue)
{
	expect(true);
	given();
}

TEST(always_true, expectFalse)
{
	expect(false);
	given();
}
