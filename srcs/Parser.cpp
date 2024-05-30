#include "Parser.hpp"

Parser::Parser(Server *server) : _server(server)
{
	_clients = &server->getClients();
	_channels = &server->getChannels();
}

Parser::~Parser() {}

void Parser::chooseParsing( Client *client, std::vector<std::string> cmd )
{
	if (cmd.size() < 1)
		throw "Invalid command (more parameters needed)";
	if (!cmd[0].compare("NICK"))
		return (nickCommand(client, cmd[1]));
	else if (!cmd[0].compare("USER"))
		return (userCommand(client, cmd));
	else if (!cmd[0].compare("CAP"))
		return ;
	else if ( client->getLogged() )
	{
		if (!cmd[0].compare("JOIN"))
			return (joinCommand(client, cmd[1]));
		else if (!cmd[0].compare("PRIVMSG"))
			return (privmsgCommand(client, cmd[1], joinString(cmd, cmd.begin() + 2)));
		// else if (!cmd[0].compare("KICK"))
		// 	return (kickCommand(client, cmd[1], cmd[2]));
		// else if (!cmd[0].compare("PART"))
		// 	return (partCommand(client, cmd[1]));
		// else if (!cmd[0].compare("WHO"))
		// 	return (whoCommand(client, cmd[1], cmd[2]));
		// else if (!cmd[0].compare("QUIT"))
		// 	throw RPL_QUIT(client->getNick(), client->getUser(), client->getHost(), joinString(cmd, cmd.begin() + 1));
		// else if (cmd[0].compare("INVITE"))
		// 	return (inviteParse(cmd));
		// else if (!cmd[0].compare("TOPIC"))
		// 	return (topicCommand(client, cmd[1], joinString(cmd, cmd.begin() + 2)));
		// else if (!cmd[0].compare("MODE"))
		// 	return (modeCommand(client, cmd));
	}
	else
		throw ERR_UNKNOWNCOMMAND(client->getNick(), cmd[0]);
}

std::map<int, Client>::iterator Parser::findNick( const std::string &nick )
{
	std::map<int, Client>::iterator it;
	for (it = _clients->begin(); it != _clients->end(); it++)
	{
		if (it->second.getNick() == nick)
			break ;
	}
	return (it);
}

void Parser::authenticateChecker( Client *client )
{
	if (client->getNick().empty() || client->getUser().empty())
		return ;
	client->setLogged(true);
	return Response::RPL_WELCOME(client);
}

// atencao ao parsing que esta a dar erro
void Parser::nickCommand( Client *client, const std::string &nickname )
{
	if (nickname.empty())
		return Response::ERR_NONICKNAMEGIVEN(client);
	
	// verify if nickname is valid
	for (unsigned long i = 0; i < nickname.size(); i++)
	{
		if ((!std::isalnum(nickname[i]) && nickname[i] != '_' && nickname[i] != '-') || nickname.size() > 9)
			return Response::ERR_ERRONEUSNICKNAME(client, nickname);
	}

	std::map<int, Client>::iterator it = findNick(nickname);
	if (it != _clients->end())
		return Response::ERR_ERRONEUSNICKNAME(client, nickname);


	log(std::string("client changed nickname to: ").append(nickname), client->getId());
	std::string oldNick = client->getNick();
	client->setNick(nickname);
	if (client->getLogged())
		return Response::RPL_NICK(client, *_clients, oldNick);
	authenticateChecker(client);
}

void Parser::userCommand( Client *client, const std::vector<std::string> &cmd )
{
	if (client->getRegistered())
		return Response::ERR_ALREADYREGISTERED(client);
	
	if (cmd.size() < 5)
		return Response::ERR_NEEDMOREPARAMS(client, "USER");

	// verify if nickname is valid
	for (unsigned long i = 0; i < cmd[1].size(); i++)
	{
		if ((!std::isalnum(cmd[1][i]) && cmd[1][i] != '_' && cmd[1][i] != '-') || cmd[1].size() > 10)
			return Response::ERR_NEEDMOREPARAMS(client, "USER");
	}

	if (cmd[2].compare("0") || cmd[3].compare("*") || (cmd[4][0] != ':' && cmd[4].size() < 2))
		return Response::ERR_NEEDMOREPARAMS(client, "USER");

	std::vector<std::string> realName;
	for (unsigned long i = 4; i < cmd.size(); i++)
	{
		if (i == 4)
			realName.push_back(&cmd[i][1]);
		else
			realName.push_back(cmd[i]);
	}

	client->setRealName(joinString(realName, realName.begin()));
	client->setUser(cmd[1]);
	if (!client->getLogged())
		authenticateChecker(client);
	client->setRegistered(true);
	log(std::string("set user to: ").append(cmd[1]), client->getId());
}

// JOIN #channel need to implement all the error messages
void Parser::joinCommand( Client *client, const std::string &channel_name )
{
	if (channel_name.empty() || channel_name.at(0) != '#')
	{
		log("no channel name given", client->getId());
		return Response::ERR_NEEDMOREPARAMS(client, "JOIN");
	}
	Channel	*channel = NULL;
	std::map<std::string, Channel>::iterator it = _channels->find(&channel_name[1]);
	if (it != _channels->end())
		channel = &it->second;
	if (!channel)
	{
		_server->createChannel(&channel_name[1], client);
		channel = &_channels->find(&channel_name[1])->second;
	}
	else
		channel->add(client);
	log("client joined channel", client->getNick(), channel->getName());
	throw RPL_JOIN(client->getNick(), client->getUser(), client->getHost(), channel->getName());
}

// TODO: verificar o parsing do target
void Parser::privmsgCommand( Client *client, const std::string &channel_name, const std::string &message )
{
	log(message);
	if (channel_name.empty() || message.empty())
	{
		log(std::string("no channel_name or message given"));
		return Response::ERR_NEEDMOREPARAMS(client, "PRIVMSG");
	}
	std::map<std::string, Channel>::iterator it = _channels->find(&channel_name[1]);
	if (it != _channels->end())
	{
		std::vector<Client *> clients = (*it).second.getClients();
		std::string msg = RPL_CHANNEL(client->getNick(), client->getUser(),client->getHost(), &channel_name[1], &message[1]);
		for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (client->getFd() != (*it)->getFd() && send((*it)->getFd(), msg.c_str(), msg.size(), 0) == -1)
				log(std::string("send problem"));
		}
	}
	log(std::string("client sent message to ").append(channel_name), client->getId());
	// throw RPL_PRIVMSG(client->getNick(), client->getUser(), channel_name, message);
}

// void Parser::partCommand( Client *client, const std::string &channel_name )
// {
// 	std::map<std::string, Channel>::iterator it = _channels->find(&channel_name[1]);
// 	if (it != _channels->end())
// 	{
// 		(*it).second.removeClient(client);
// 		(*it).second.removeOperator(client);
// 		// erase all clients?
// 		if ((*it).second.getClients().empty())
// 			_channels->erase(it);
// 		throw RPL_PART(client->getNick(), client->getUser(), client->getHost(), &channel_name[1], "parting channel");
// 	}
// 	throw ERR_NOSUCHCHANNEL(client->getNick(), &channel_name[1]);
// }

// TODO: adicionar parsing do comentario como argumento ou nao e verificar o mode do channel
// void Parser::kickCommand( Client *client, const std::string &channel_name, const std::string &username ) 
// {
// 	std::map<std::string, Channel>::iterator it = _channels->find(channel_name);
// 	if (it != _channels->end())
// 		throw "não encontrou channel";
// 	std::vector<Client *> clients = (*it).second.getClients();
// 	for (unsigned long i = 0; i < clients.size(); i++)
// 	{
// 		if (clients[i]->getUser() == username)
// 		{
// 			(*it).second.removeClient(clients[i]);
// 			throw RPL_KICK(client->getNick(), client->getUser(), channel_name, username);
// 		}
// 	}
// 	throw "client not found";
	// std::map<std::string, Channel>::iterator it;
	// it = _server->_channels.find(token[1]);
	// if (it != server._channels.end())
	// {
	// 	std::vector<Client *> channel_clients;
		
	// 	channel_clients = it->second.getClients();
	// 	for (unsigned long i = 0; i < channel_clients.size(); i++)
	// 	{
	// 		if (channel_clients[i] == token[2])
	// 		{
	// 			std::vector<Client *> operators;
	// 			operators = it->second.getOperators();
	// 			for (unsigned long j = 0; j < operators.size(); j++)
	// 			{
	// 				if (operators[i] == client)
	// 				{
	// 					it->second.removeClient(channel_clients[i])

	// 				}
	// 			}
	// 		}
	// 	}
	// 	command_error;
	// }

// int Parser::InviteParse(std::vector<std::string> tokens, Client *client, Channel *channel, Server *server)
// {
// 	Client	*invited;
// 	invited = server.getClient(token[1]);
// 	int	able = 1;
// 	if (invited != 0)
// 	{
// 		std::map<std::string, Channel>::iterator it;
// 		it = server._channels.find(token[2]);
// 		if (it != server._channels.end())
// 		{
// 			std::vector<Client *> channel_clients;
		
// 			channel_clients = it->second.getClients();
// 			for (unsigned long i = 0; i < channel_clients.size(); i++)
// 			{
// 				if (channel_clients[i] == invited)
// 				{
// 					already in the channel;
// 				}
// 			}
// 			if (it->second.getT() == true)
// 			{
// 				able = 0;
// 				std::vector<Client *> operators;
// 				operators = it->second.getOperators();
// 				for (unsigned long j = 0; j < operators.size(); j++)
// 				{
// 					if (operators[i] == client)
// 					able = 1;
// 				}
// 			}
// 			if (it->second.getLimit() < it->second._clients.size() && able == 1)
// 			{
// 				it->second.addClient(invited);
// 			}
// 			else
// 			{
// 				channel full or not operator;
// 			}
// 			}
// 		}
// 	}

// 	else
// 		user doesnt exist


// }

// void	Parser::topicCommand( Client *client, const std::string &channel_name, const std::string &topic ) 
// {
// 	std::map<std::string, Channel>::iterator it;
// 	it = _channels->find(&channel_name[1]);
// 	int	able = true;
// 	if (it != _channels->end())
// 	{
// 		if (it->second.getT())
// 		{
// 			able = false;
// 			std::vector<Client *> operators;
// 			operators = it->second.getOperators();
// 			for (unsigned long j = 0; j < operators.size(); j++)
// 			{
// 				if (operators[j] == client)
// 					able = true;
// 			}
// 		}
// 		if (able)
// 		{
// 			if (!topic.empty() && topic.at(0) == ':' && topic.size() > 1)
// 				(*it).second.setTopic(&topic[1]);
// 			else if (!topic.empty() && topic.at(0) == ':')
// 				(*it).second.setTopic("");
// 		}
// 		throw RPL_TOPIC(client->getNick(), client->getUser(), client->getHost(), &channel_name[1], (*it).second.getTopic());
// 	}
// 	throw ERR_NOSUCHCHANNEL(client->getNick(), &channel_name[1]);
// }

// void Parser::modeCommand( Client *client, const std::vector<std::string> &cmd ) 
// {
// 	std::string channel_name = cmd[1];
// 	std::map<std::string, Channel>::iterator it = _channels->find(&channel_name[1]);
	
// 	if (it == _channels->end())
// 		throw ERR_NOSUCHCHANNEL(client->getNick(), &channel_name[1]);

// 	std::vector<Client *> operators = it->second.getOperators();
// 	std::vector<Client *>::iterator ito = operators.begin();
// 	for (;ito != operators.end() ; ito++)
// 	{
// 		if (*ito == client)
// 			break;
// 	}
// 	if (ito != operators.end())
// 	{
// 		if (cmd.size() < 3)
// 			return (Response::message(client, "324 " + client->getNick() + " " + channel_name + " -itko"));
// 		if (!cmd[2].compare("+i"))
// 			it->second.setI(true);
// 		else if (!cmd[2].compare("-i"))
// 			it->second.setI(false);
// 		else if (!cmd[2].compare("+t"))
// 			it->second.setT(true);
// 		else if (!cmd[2].compare("-t"))
// 			it->second.setT(false);
// 		else if (!cmd[2].compare("+k"))
// 		{
// 			it->second.setK(true);
// 			if (!cmd[3].empty())
// 				it->second.setPw(cmd[3]);
// 			else
// 				it->second.setPw("");
// 		}
// 		else if (!cmd[2].compare("-k"))
// 		{
// 			if (!cmd[3].empty() && cmd[3].compare(it->second.getPw()))
// 			{
// 				it->second.setK(false);
// 				it->second.setPw("");
// 			}
// 			else
// 				throw ERR_BADCHANNELKEY(client->getNick(), &channel_name[1]);
// 		}
// 		else if (!cmd[2].compare("+o"))
// 		{
// 			Client *client = &findNick(cmd[3])->second;
// 			std::vector<Client *>::iterator it_add_operator = operators.begin();
// 			for (; it_add_operator != operators.end(); it_add_operator++)
// 			{
// 				if (*it_add_operator == client)
// 					break ;
// 			}
// 			if (it_add_operator != operators.end())
// 				(*it).second.addOperator(client);
// 		}
// 		else if (!cmd[2].compare("+l"))
// 		{
// 			(*it).second.setL(true);
// 			if (cmd[3].empty())
// 				throw ERR_NOSUCHCHANNEL(client->getNick(), &channel_name[1]);
// 			for (int j = 0; cmd[3][j]; j++)
// 			{
// 				if (!isdigit(cmd[3][j]))
// 					throw ERR_NEEDMOREPARAMS(client->getNick(), "MODE");
// 			}
// 			it->second.setLimit(atoi(cmd[3].c_str()));
// 		}
// 		else if (!cmd[2].compare("-l"))
// 			it->second.setL(false);
// 		else
// 		{
// 			DEBUG("foi aqui");
// 			throw ERR_UMODEUNKNOWNFLAG(client->getNick());
// 		}
// 	}
// }

// // only works to list client on specific channel
// void Parser::whoCommand( Client *client, const std::string &mask, const std::string &o )
// {
// 	(void)o;
// 	if (mask.size() > 1 && mask.at(0) == '#')
// 	{
// 		std::map<std::string, Channel>::iterator it = _channels->find(&mask[1]);
// 		if (it != _channels->end())
// 		{
// 			std::vector<Client *> clients = it->second.getClients();
// 			for (unsigned long i = 0; i < clients.size(); i++)
// 				Response::message(client, "352 " + clients[i]->getNick() + " " + mask + " " + clients[i]->getUser() + " " + clients[i]->getHost() + " " + clients[i]->getNick() + " H :0 ");
// 			Response::message(client, "315 " + client->getNick() + " " + mask + " :End of /WHO list.");
// 		}
// 	}
// }

