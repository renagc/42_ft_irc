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
		if (client != clients[i])
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
	std::string msg = ":" + userPrefix(client) + " " + response;
	if (send(client->getFd(), msg.c_str(), msg.size(), 0) == -1)
		std::cerr << "send problem" << std::endl;
}

void Response::ircMessage( Client *client, const std::string &response )
{
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
void Response::ERR_NOTONCHANNEL( Client *client, const std::string &channel ) { numericReply(client, "442", strtov(1, channel.c_str()), "You're not on that channel"); }
void Response::ERR_INVITEONLYCHAN( Client *client, const std::string &channel ) { numericReply(client, "473", strtov(1, channel.c_str()), "Cannot join channel (+i)"); }
void Response::ERR_CHANNELISFULL( Client *client, const std::string &channel ) { numericReply(client, "471", strtov(1, channel.c_str()), "Cannot join channel (+l)"); }
void Response::ERR_BADCHANNELKEY( Client *client, const std::string &channel ) { numericReply(client, "475", strtov(1, channel.c_str()), "Cannot join channel (+k)"); }

// PART
void Response::RPL_PART( Client *client, Channel *channel, const std::string &message )
{
	Response::message(client, "PART #" + channel->getName() + " :" + message + "\r\n");
	broadcastChannel(client, channel, "PART #" + channel->getName() + " :" + message + "\r\n");
}

// JOIN
void Response::RPL_JOIN( Client *client, Channel *channel )
{
	Response::message(client, "JOIN #" + channel->getName() + "\r\n");
	broadcastChannel(client, channel, "JOIN #" + channel->getName() + "\r\n");
}

void Response::RPL_TOPIC( Client *client, const std::string &channel, const std::string &topic ) { Response::message(client, "TOPIC #" + channel + " :" + topic + "\r\n"); }
void Response::RPL_NAMREPLY( Client *client, const std::string &channel, const std::string &users )
{
	std::string new_channel = "#" + channel;
	Response::numericReply(client, "353", strtov(2, "=", new_channel.c_str()), users);
}
void Response::ERR_BANNEDFROMCHAN( Client *client, const std::string &channel ) { numericReply(client, "474", strtov(1, channel.c_str()), "Cannot join channel (+b)"); }

// QUIT
void Response::RPL_QUIT( Client *client, Channel *channel, const std::string &message ) { Response::broadcastChannel(client, channel, "QUIT :" + message + "\r\n"); }

// KICK
void Response::ERR_CHANOPRIVSNEEDED( Client *client, const std::string &channel ) { numericReply(client, "482", strtov(1, channel.c_str()), "You're not channel operator"); }
void Response::RPL_KICK( Client *client, Channel *channel, const std::string &nickname )
{
	Response::broadcastChannel(client, channel, "KICK #" + channel->getName() + " " + nickname + "\r\n");
}
void Response::ERR_USERNOTINCHANNEL( Client *client, const std::string &channel, const std::string &nickname ) { numericReply(client, "441", strtov(2, nickname.c_str(), channel.c_str()), "They aren't on that channel"); }

// MODE
void Response::ERR_UMODEUNKNOWNFLAG( Client *client ) { numericReply(client, "501", std::vector<std::string>(), "Unknown MODE flag"); }
void Response::RPL_CHANNELMODEIS( Client *client, Channel *channel )
{
	ircMessage(client, ":localhost 324 " + client->getNick() + " #" + channel->getName() + " " + channel->getMode() + "\r\n");
}
void Response::ERR_KEYSET( Client *client, const std::string &channel ) { numericReply(client, "467", strtov(1, channel.c_str()), "Channel key already set"); }
void Response::ERR_UNKNOWNMODE( Client *client, char c, const std::string &channel ) { numericReply(client, "472", strtov(2, std::string(1, c).c_str(), channel.c_str()), "is unknown mode char to me for " + channel); }