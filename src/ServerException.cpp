#include "Server.hpp"

const char		*Server::GetAddressFailException::what() const throw()
{
	return ("ServerException:: Get address info fail");
}

const char		*Server::SocketOpenFailException::what() const throw()
{
	return ("ServerException:: Socket open fail");
}

const char		*Server::SocketBindFailException::what() const throw()
{
	return ("ServerException:: Socket bind fail");
}

const char		*Server::SocketListenFailException::what() const throw()
{
	return ("ServerException:: Socket listen fail");
}

const char		*Server::SelectFailException::what() const throw()
{
	return ("ServerException:: Select fail");
}

const char		*Server::AcceptFailException::what() const throw()
{
	return ("ServerException:: Accept fail");
}
