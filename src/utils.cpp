#include "utils.hpp"

static int			ft_check_range(unsigned long long number, int sign)
{
	if (number > LLONG_MAX - 1 && sign == -1)
		return (0);
	if (number > LLONG_MAX && sign == 1)
		return (-1);
	return (number * sign);
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

bool			isInTheMask(const char &target, const char *mask)
{
	int i;

	i = 0;
	while (mask[i])
	{
		if (target == mask[i])
			return (true);
		++i;
	}
	return (false);
}

bool            stringCheck(const char *str, const char *mask, const char &notRepeatChar)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (!isInTheMask(str[i], mask))
			return (false);
		if (str[i + 1] && str[i] == notRepeatChar && str[i + 1] == notRepeatChar)
			return (false);
		++i;
	}
	return (true);
}

bool				ft_isdigit(const char *str)
{
	int			i;

	i = -1;
	while (str[++i])
	{
		if (!('0' <= str[i] && str[i] <= '9'))
			return (false);
	}
	return (true);
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

bool				isValidFormat(const std::string &key, const char &value)
{
	if (std::string::npos == key.find(value))
		return (false);
	return (true);
}

std::string		getTimestamp(std::time_t &startTime, const bool &forUptime)
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
		uptime.tm_year = 1970;
		uptime.tm_mon = 1;
		uptime.tm_mday = 1;
		uptime.tm_hour = 9;
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
	if (!stringCheck(address.substr(portIdx + 1, passIdx - portIdx - 1).c_str(), "0123456789", '.')) // 반복되지 말아야 할 문자가 없으므로 상관없는 문자를 넣어둠
		return (false);
	if (!stringCheck(address.substr(0, portIdx - 1).c_str(), ".0123456789", '.'))
		return (false);
	return (passIdx != std::string::npos && portIdx != std::string::npos);
}