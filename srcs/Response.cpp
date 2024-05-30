#include "Response.hpp"

Response::~Response() {}

std::string Response::userPrefix( Client *client )
{
	return (client->getNick() + "!~" + client->getUser() + "@" + client->getHost());
}

void Response::broadcastChannel( Client *client, Channel *channel, const std::string &response )
{
	std::string msg = ":" + userPrefix(client) + " " + response;
	std::vector<Client *> clients = channel->getClients();
	for (unsigned long i = 0; i < clients.size(); i++)
	{
		if (clients[i] != client)
		{
			if (send(clients[i]->getFd(), msg.c_str(), msg.size(), 0) == -1)
				std::cerr << "send problem" << std::endl;
		}
	}
}

void Response::broadcastAll( const std::map<int, Client> &clients, const std::string &response )
{
	std::map<int, Client>::const_iterator itc = clients.begin();
	for (; itc != clients.end(); itc++)
	{
		if (send((*itc).second.getFd(), response.c_str(), response.size(), 0) == -1)
			std::cerr << "send problem" << std::endl;
	}
}

void Response::message( Client *client, const std::string &response )
{
	std::string msg = userPrefix(client) + " " + response;
	if (send(client->getFd(), response.c_str(), response.size(), 0) == -1)
		std::cerr << "send problem" << std::endl;
}
void Response::numericReply( Client *client, const std::string &code, const std::vector<std::string> &args, const std::string &message )
{
	std::string msg = ":localhost " + code + " " + client->getNick();

	for (unsigned long i = 0; i < args.size(); i++)
		msg += " " + args[i];
	msg += + " :" + message + "\r\n";
	if (send(client->getFd(), msg.c_str(), msg.size(), 0) == -1)
		std::cerr << "send problem" << std::endl;
}

// NICK
void Response::ERR_NONICKNAMEGIVEN( Client *client ) { numericReply(client, "431", std::vector<std::string>(), "No nickname given"); }
void Response::ERR_ERRONEUSNICKNAME( Client *client, const std::string &nickname ) { numericReply(client, "432", strtov(1, nickname.c_str()),"Erroneous nickname"); }
void Response::ERR_NICKNAMEINUSE( Client *client, const std::string &nickname ) { numericReply(client, "433", strtov(1, nickname.c_str()), "Nickname is already in use"); }
void Response::RPL_NICK( Client *client, const std::map<int, Client> &clients, const std::string &oldNick ) { broadcastAll(clients, ":" + oldNick + "!" + client->getUser() + "@" + client->getHost() + " NICK :" + client->getNick() + "\r\n"); }
void Response::ERR_ALREADYREGISTERED( Client *client ) { numericReply(client, "462", std::vector<std::string>(), "You may not reregister"); }
void Response::ERR_NEEDMOREPARAMS( Client *client, const std::string &command ) { numericReply(client, "461", strtov(1, command.c_str()), "Not enough parameters"); }
void Response::RPL_WELCOME( Client *client ) { numericReply(client, "001", std::vector<std::string>(), "Welcome to the Internet Relay Network " + client->getNick() + "!" + client->getUser() + "@" + client->getHost()); }

// CHANNEL
void Response::ERR_NOSUCHCHANNEL( Client *client, const std::string &channel ) { numericReply(client, "403", strtov(1, channel.c_str()), "No such channel"); }