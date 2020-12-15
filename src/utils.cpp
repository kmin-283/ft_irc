#include "utils.hpp"

bool			ft_isdigit(char *str)
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

void			*ft_memset(void *s, int c, size_t n)
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

bool			isValidFormat(const std::string &key, const char &value)
{
	if (std::string::npos == key.find(value))
		return (false);
	return (true);
}

bool			isValidNickname(const Message &message)
{
	for (size_t i = 0; i < message.getParameter(0).length(); i++)
	{
		if (i == 0 && !isValidFormat(std::string(LETTER) + std::string(SPECIAL), message.getParameter(0)[i]))
			return false;
		else if (!isValidFormat(std::string(LETTER) + std::string(SPECIAL) + std::string(DIGIT), message.getParameter(0)[i]))
			return false;
	}
	return true;
}
