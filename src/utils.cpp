#include "utils.hpp"

static int			ft_check_range(unsigned long long number, int sign)
{
	if (number > LLONG_MAX - 1 && sign == -1)
		return (0);
	if (number > LLONG_MAX && sign == 1)
		return (-1);
	return (number * sign);
}

bool 		match(const char *first, const char *second)
{
	if (*first == 0 && *second == 0)
		return (true);
	if (*first == '*' && *(first + 1) != '\0' && *second == '\0')
		return (false);
	if (*first == '?' || *first == *second)
		return match(first + 1, second + 1);
	if (*first == '*')
		return match(first + 1, second) || match(first, second + 1);
	if (*first == '&' && (*(first + 1) == '.' || *(first + 1) == 0) && *second != '.' && *second != 0)
		return match(first, second + 1);
	if (*first == '&' && (*(first + 1) == '.' || *(first + 1) == 0) && (*second == '.' || *second == 0))
		return match(first + 1, second);
	return (false);
}

int					ft_atoi(const char *str)
{
	unsigned long long	number_value;
	int					sign_value;
	int					i;

	i = 0;
	number_value = 0;
	sign_value = 1;
	while ((*(str + i) == '\t' || *(str + i) == '\n'
		|| *(str + i) == '\v' || *(str + i) == '\f'
		|| *(str + i) == '\r' || *(str + i) == ' ') && *(str + i))
		i++;
	if (*(str + i) == '-')
		sign_value = -1;
	if (*(str + i) == '-' || *(str + i) == '+')
		i++;
	while (('0' <= *(str + i) && *(str + i) <= '9') && *(str + i))
	{
		number_value = number_value * 10 + (*(str + i) - '0');
		i++;
	}
	return (ft_check_range(number_value, sign_value));
}

bool			isInTheMask(const std::string &mask, const char &target)
{
	return (mask.find(target) != std::string::npos);
}

bool            isValidPort(const std::string &port)
{
	size_t portLength;

	portLength = port.length();
	if (!portLength)
		return (false);
	for (size_t i = 0; i < portLength; ++i)
	{
		if (!isInTheMask(std::string(DIGIT), port[i]))
			return (false);
	}
	return (true);
}

bool			isValidIpv4(const std::string &ipAddress)
{
	int		cnt;
	size_t	ipAddressLength;
	int		hasPoint;

	cnt = 0;
	ipAddressLength = ipAddress.length();
	hasPoint = 0;
	if (!ipAddressLength)
		return (false);
	for (size_t i = 0; i < ipAddressLength; ++i)
	{
		if (isInTheMask(std::string(DIGIT), ipAddress[i]))
			cnt += 1;
		if (cnt > 3)
			return (false);
		if (ipAddress[i] == '.')
		{
			if (i == 0 || ipAddress[i + 1] == '.' || ipAddress[i + 1] == 0)
				return (false);
			hasPoint += 1;
			cnt = 0;
		}
	}
	return (hasPoint == 3 ? true : false);
}

void				*ft_memset(void *s, int c, size_t n)
{
		size_t	i;
		char	*str;

		str = (char *)s;
		i = -1;
		while (++i < n)
			*(str + i) = c;
		s = str;
		return (s);
}

std::string		getTimestamp(const std::time_t &startTime, const bool &forUptime)
{
	struct tm	*parsedTime;
	std::string	returnString;
	struct tm	uptime;

	if (forUptime)
	{
		uptime.tm_year = 1970;
		uptime.tm_mon = 1;
		uptime.tm_mday = 1;
		uptime.tm_hour = 9;
	}
	else
	{
		uptime.tm_year = 0;
		uptime.tm_mon = 0;
		uptime.tm_mday = 0;
		uptime.tm_hour = 0;
	}
	parsedTime = localtime(&startTime);
	returnString = std::to_string(1900 + parsedTime->tm_year - uptime.tm_year);
	returnString += "/";
	if (parsedTime->tm_mon + 1 < 10)
		returnString += "0";
	returnString += std::to_string(parsedTime->tm_mon + 1 - uptime.tm_mon);
	returnString += "/";
	if (parsedTime->tm_mday < 10)
		returnString += "0";
	returnString += std::to_string(parsedTime->tm_mday - uptime.tm_mday);
	returnString +=  " at ";
	if (parsedTime->tm_hour < 10)
		returnString +=  "0";
	returnString += std::to_string(parsedTime->tm_hour - uptime.tm_hour);
	returnString += ":";
	if (parsedTime->tm_min < 10)
		returnString +=  "0";
	returnString += std::to_string(parsedTime->tm_min);
	returnString += ":";
	if (parsedTime->tm_sec < 10)
		returnString +=  "0";
	returnString += std::to_string(parsedTime->tm_sec);
	returnString += " (KST)";
	return (returnString);
}

bool			isValidAddress(const std::string &address)
{
	size_t	passIdx;
	size_t	portIdx;

	passIdx = address.rfind(':');
	portIdx = address.rfind(':', passIdx - 1);
	if (!isValidPort(address.substr(portIdx + 1, passIdx - portIdx - 1)))
		return (false);
	if (!isValidIpv4(address.substr(0, portIdx)))
		return (false);
	return (passIdx != std::string::npos && portIdx != std::string::npos);
}