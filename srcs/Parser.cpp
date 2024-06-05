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
	if (!cmd[0].compare("CAP"))
	{
		if (!cmd[1].compare("LS"))
			return Response::ircMessage(client, ":localhost CAP * LS :multi-prefix sasl\r\n");
		else if (!cmd[1].compare("REQ") || !cmd[1].compare("END"))
			return Response::ircMessage(client, ":localhost CAP * ACK :multi-prefix sasl\r\n");
	}
	else if (!cmd[0].compare("NICK"))
		return (nickCommand(client, cmd[1]));
	else if (!cmd[0].compare("USER"))
		return (userCommand(client, cmd));
	else if ( client->getLogged() )
	{
		if (!cmd[0].compare("JOIN"))
			return (joinCommand(client, cmd));
		else if (!cmd[0].compare("PRIVMSG"))
			return (privmsgCommand(client, cmd[1], joinString(cmd, cmd.begin() + 2)));
		else if (!cmd[0].compare("KICK"))
			return (kickCommand(client, cmd));
		else if (!cmd[0].compare("PART"))
			return (partCommand(client, cmd));
		// else if (!cmd[0].compare("WHO"))
		// 	return (whoCommand(client, cmd[1], cmd[2]));
		else if (!cmd[0].compare("QUIT"))
			return quitCommand(client, cmd[1]);
		else if (!cmd[0].compare("INVITE"))
			return (inviteCommand(client, cmd));
		else if (!cmd[0].compare("TOPIC"))
			return (topicCommand(client, cmd));
		else if (!cmd[0].compare("MODE"))
			return (modeCommand(client, cmd));
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
void Parser::joinCommand( Client *client, const std::vector<std::string> &cmd )
{
	if (cmd.size() < 2)
		return Response::ERR_NEEDMOREPARAMS(client, "JOIN");
	if (!cmd[1].compare("0"))
	{
		std::map<std::string, Channel>::iterator it = _channels->begin();
		for (; it != _channels->end(); it++)
		{
			Channel *channel = &it->second;
			std::string channel_name = "#" + channel->getName();
			if (channel->findClient(client))
				partCommand(client, strtov(3, "PART", channel_name.c_str(), "join 0 pressed"));
		}
		return ;
	}
	std::vector<std::string> channels = split(cmd[1], ",");
	std::vector<std::string> keys;
	if (cmd.size() > 2)
		keys = split(cmd[2], ",");
	for (unsigned long i = 0; i < channels.size(); i++)
	{
		if (channels[i].at(0) != '#')
		{
			Response::ERR_NOSUCHCHANNEL(client, channels[i]);
			continue ;
		}
		std::map<std::string, Channel>::iterator it = _channels->find(&channels[i][1]);
		if (it == _channels->end())
		{
			if (channels[i].size() > 50 || channels[i].find(" ") != std::string::npos || channels[i].find("\a") != std::string::npos || channels[i].find(",") != std::string::npos)
			{
				Response::ERR_NOSUCHCHANNEL(client, &channels[i][1]);
				continue ;
			}
			_server->createChannel(&channels[i][1], client);
		}
		else
		{
			Channel *channel = &it->second;
			if (channel->getI() && !channel->isInvited(client))
			{
				Response::ERR_INVITEONLYCHAN(client, channel->getName());
				continue ;
			}
			else if (channel->getL() && channel->getLimit() < (int)channel->getClients().size())
			{
				Response::ERR_CHANNELISFULL(client, channel->getName());
				continue ;
			}
			else if (channel->getK())
			{
				if (keys.size() < i + 1 || keys[i].compare(channel->getPw()))
				{
					Response::ERR_BADCHANNELKEY(client, channel->getName());
					continue ;
				}
			}
			if (channel->isKicked(client))
			{
				Response::ERR_BANNEDFROMCHAN(client, channel->getName());
				continue ;
			}
			channel->add(client);
		}
		Channel *final_channel = &_channels->find(&channels[i][1])->second;
		log("client joined channel", client->getNick(), &channels[i][1]);
		Response::RPL_JOIN(client, final_channel);
		Response::RPL_TOPIC(client, final_channel->getName(), final_channel->getTopic());
		Response::RPL_NAMREPLY(client, final_channel->getName(), final_channel->getUsers());
	}
}

void Parser::kickCommand( Client *client, const std::vector<std::string> &cmd )
{
	if (cmd.size() < 3) // parametros insuficientes
		return Response::ERR_NEEDMOREPARAMS(client, "KICK");
	std::vector<std::string> channels = split(cmd[1], ",");
	std::vector<std::string> users = split(cmd[2], ",");
	for (unsigned long i = 0; i < channels.size(); i++)
	{
		if (channels[i][0] != '#') // sem #
		{
			Response::ERR_NOSUCHCHANNEL(client, channels[i]);
			continue ;
		}
		std::map<std::string, Channel>::iterator it = _channels->find(&channels[i][1]);
		if (it == _channels->end())
		{
			Response::ERR_NOSUCHCHANNEL(client, &channels[i][1]);
			continue ;
		}
		if (!(*it).second.isOperator(client))
		{
			Response::ERR_CHANOPRIVSNEEDED(client, channels[i]);
			continue ;
		}
		if (users.size() < i + 1) // sem user
		{
			Response::ERR_NEEDMOREPARAMS(client, "KICK");
			continue ;
		}
		std::vector<Client *> clientsOnChannel = (*it).second.getClients();
		if (!(*it).second.findClient(client))
		{
			Response::ERR_USERNOTINCHANNEL(client, &channels[i][1], users[i]);
			continue ;
		}
		log("client kicked from channel");
		for (unsigned long j = 0; j < clientsOnChannel.size(); j++)
		{
			if (clientsOnChannel[j]->getNick() == users[i])
			{
				Response::message(client, "KICK #" + (*it).second.getName() + " " + users[i] + "\r\n");
				Response::RPL_KICK(client, &(*it).second, users[i]);
				(*it).second.removeClient(clientsOnChannel[j]);
				(*it).second.removeOperator(clientsOnChannel[j]);
				(*it).second.addKicked(clientsOnChannel[j]);
				break ;
			}
		}
	}
}

void Parser::quitCommand( Client *client, const std::string &message )
{
	std::map<std::string, Channel>::iterator it = _channels->begin();
	for (; it != _channels->end(); it++)
	{
		Channel *channel = &it->second;
		if (channel->findClient(client))
			Response::RPL_QUIT(client, channel, &message[1]);
	}
	Response::message(client, "QUIT " + message + "\r\n");
	_server->clientDisconnect(client);
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
}

void Parser::partCommand( Client *client, const std::vector<std::string> &cmd )
{
	std::string partMessage = "default part message";
	if (cmd.size() < 2)
		return Response::ERR_NEEDMOREPARAMS(client, "PART");
	if (cmd.size() >= 3 && cmd[2].at(0) == ':')
	{
		partMessage = joinString(cmd, cmd.begin() + 2);
		partMessage = &partMessage[1];
	}
	if (cmd[1].find(',') != std::string::npos)
	{
		std::vector<std::string> channels = split(cmd[1], ",");
		for (unsigned long i = 0; i < channels.size(); i++)
		{
			std::map<std::string, Channel>::iterator it = _channels->find(&channels[i][1]);
			if (it == _channels->end())
				Response::ERR_NOSUCHCHANNEL(client, &channels[i][1]);
			if (!(*it).second.findClient(client))
				return Response::ERR_NOTONCHANNEL(client, channels[i]);
			(*it).second.removeClient(client);
			(*it).second.removeOperator(client);
			Response::RPL_PART(client, &(*it).second, partMessage);
			if (!(*it).second.getClients().size() && !(*it).second.getOperators().size())
				_channels->erase(it);
		}
		return ;
	}
	if (cmd[1].at(0) != '#')
		return Response::ERR_NEEDMOREPARAMS(client, cmd[1]);
	else
	{
		std::map<std::string, Channel>::iterator it = _channels->find(&cmd[1][1]);
		if (it == _channels->end())
			return Response::ERR_NOSUCHCHANNEL(client, &cmd[1][1]);
		if (!(*it).second.findClient(client))
			return Response::ERR_NOTONCHANNEL(client, (*it).second.getName());
		(*it).second.removeClient(client);
		(*it).second.removeOperator(client);
		Response::RPL_PART(client, &(*it).second, partMessage);
		if (!(*it).second.getClients().size() && !(*it).second.getOperators().size())
			_channels->erase(it);
	}
}

void Parser::modeCommand( Client *client, const std::vector<std::string> &cmd )
{
	if (cmd.size() < 2) 
		return Response::ERR_NEEDMOREPARAMS(client, "MODE");
	if (cmd[1][0] != '#')
		return Response::ERR_NOSUCHCHANNEL(client, &cmd[1][1]);
	
	// channel nao existe
	std::map<std::string, Channel>::iterator it = _channels->find(&cmd[1][1]);
	if (it == _channels->end()) 
		return Response::ERR_NOSUCHCHANNEL(client, &cmd[1][1]);

	// sem args
	if (cmd.size() < 3)
		return Response::RPL_CHANNELMODEIS(client, &it->second);

	if (!it->second.isOperator(client))
		return Response::ERR_CHANOPRIVSNEEDED(client, "#" + it->second.getName());
	unsigned long initial_params = 3;
	unsigned long params = 3;
	for (unsigned long i = 2; i < cmd.size(); i++)
	{
		if (cmd[i].at(0) != '+' && cmd[i].at(0) != '-')
			return Response::ERR_UMODEUNKNOWNFLAG(client);
		bool signal = cmd[i].at(0) == '+';
		for (unsigned long j = 1; j < cmd[i].size(); j++)
		{
			if (cmd[i][j] == 'i')
			{
				if (it->second.getI() != signal)
				{
					it->second.setI(signal);
					Response::message(client, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "i\r\n");
					Response::broadcastChannel(client, &it->second, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "i " + "\r\n");
				}
			}
			else if (cmd[i][j] == 't')
			{
				if (it->second.getT() == !signal)
				{
					it->second.setT(signal);
					Response::message(client, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "t\r\n");
					Response::broadcastChannel(client, &it->second, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "t " + "\r\n");
				}
			}
			else if (cmd[i][j] == 'k')
			{
				if (params >= cmd.size())
					Response::ERR_NEEDMOREPARAMS(client, "MODE");
				else if (it->second.getK())
				{
					if (!signal && !it->second.getPw().compare(cmd[params]))
					{
						it->second.setK(signal);
						it->second.setPw("");
						Response::message(client, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "k " + cmd[params] + "\r\n");
						Response::broadcastChannel(client, &it->second, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "k " + cmd[params] + "\r\n");
					}
					else
						Response::ERR_KEYSET(client, it->second.getName());
				}
				else
				{
					it->second.setK(signal);
					it->second.setPw(cmd[params]);
					Response::message(client, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "k " + cmd[params] + "\r\n");
					Response::broadcastChannel(client, &it->second, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "k " + cmd[params] + "\r\n");
				}
				params++;
			}
			else if (cmd[i][j] == 'l')
			{
				if (it->second.getL() && !signal)
				{
					it->second.setL(signal);
					it->second.setLimit(-1);
					Response::message(client, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "l\r\n");
					Response::broadcastChannel(client, &it->second, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "l " + "\r\n");
				}
				else if (signal)
				{
					if (params >= cmd.size())
						Response::ERR_NEEDMOREPARAMS(client, "MODE");
					else
					{
						unsigned long k = 0;
						for (; k < cmd[params].size(); k++)
						{
							if (!std::isdigit(cmd[params][k]))
								break ;
						}
						if (k != cmd[params].size())
							Response::ERR_NEEDMOREPARAMS(client, "MODE");
						else if (atoi(cmd[params].c_str()) > 0 && atoi(cmd[params].c_str()) != it->second.getLimit())
						{
							it->second.setL(signal);
							it->second.setLimit(atoi(cmd[params].c_str()));
							Response::message(client, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "l " + cmd[params] + "\r\n");
							Response::broadcastChannel(client, &it->second, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "l " + cmd[params] + "\r\n");
						}
					}
					params++;
				}
			}
			else if (cmd[i][j] == 'o')
			{
				if (params >= cmd.size())
					Response::ERR_NEEDMOREPARAMS(client, "MODE");
				else
				{
					Client *op = NULL;
					std::vector<Client *> clients = it->second.getClients();
					for (unsigned long i = 0; i < clients.size(); i++)
					{
						if (!clients[i]->getNick().compare(cmd[params]))
						{
							op = clients[i];
							break ;
						}
					}
					if (op == NULL)
						Response::ERR_NEEDMOREPARAMS(client, "MODE");
					else
					{
						if (signal)
						{
							if (!it->second.isOperator(op))
							{
								it->second.addOperator(op);
								Response::message(client, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "o " + cmd[params] + "\r\n");
								Response::broadcastChannel(client, &it->second, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "o " + cmd[params] + "\r\n");
							}
						}
						else
						{
							if (it->second.isOperator(op))
							{
								it->second.removeOperator(op);
								Response::message(client, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "o " + cmd[params] + "\r\n");
								Response::broadcastChannel(client, &it->second, "MODE #" + it->second.getName() + " " + cmd[i].at(0) + "o " + cmd[params] + "\r\n");
							}
						}
					}
				}
			}
			else
				Response::ERR_UNKNOWNMODE(client, cmd[i][j], it->second.getName());
		}
		if (initial_params != params)
		{
			initial_params = params;
			i = params - 1;
		}
		initial_params++;
		params++;
	}
}

void Parser::inviteCommand( Client *client, const std::vector<std::string> &cmd )
{
	if (cmd.size() < 3)
		return Response::ERR_NEEDMOREPARAMS(client, "INVITE");
	if (cmd[2][0] != '#')
		return Response::ERR_NOSUCHCHANNEL(client, cmd[2]);
	std::map<std::string, Channel>::iterator it = _channels->find(&cmd[2][1]);
	if (it == _channels->end()) 
		return Response::ERR_NOSUCHCHANNEL(client, &cmd[2][1]);

	if (!it->second.isOperator(client))
		return Response::ERR_CHANOPRIVSNEEDED(client, "#" + it->second.getName());

	std::vector<Client *> clients = it->second.getClients();
	unsigned long i = 0;
	for (; i < clients.size(); i++)
	{
		if (clients[i] == client)
			break ;
	}
	if (clients[i] != client)
		return Response::ERR_NOTONCHANNEL(client, &cmd[2][1]);

	std::map<int, Client>::iterator itc = _clients->begin();
	for (; itc != _clients->end(); itc++)
	{
		if (!itc->second.getNick().compare(cmd[1]))
			break ;
	}
	if (itc == _clients->end())
		return Response::ERR_NOSUCHNICK(client, cmd[1]);
	
	if (it->second.findClient(&itc->second))
		return Response::ERR_USERONCHANNEL(client, cmd[1], it->second.getName());

	it->second.addInvited(&itc->second);
	Response::RPL_INVITING(client, itc->second.getNick(), it->second.getName());
	Response::ircMessage(&itc->second, ":" + Response::userPrefix(&itc->second) + " INVITE " + itc->second.getNick() + " #" + it->second.getName() + "\r\n");
}

void	Parser::topicCommand( Client *client, const std::vector<std::string> &cmd ) 
{
	if (cmd.size() < 2)
		return Response::ERR_NEEDMOREPARAMS(client, "TOPIC");
	if (cmd[1].at(0) != '#')
		return Response::ERR_NOSUCHCHANNEL(client, &cmd[1][1]);
	
	std::map<std::string, Channel>::iterator it;
	it = _channels->find(&cmd[1][1]);
	if (it == _channels->end())
		return Response::ERR_NOSUCHCHANNEL(client, &cmd[1][1]);
	
	std::vector<Client *> clients = it->second.getClients();
	unsigned long i = 0;
	for (; i < clients.size(); i++)
	{
		if (clients[i] == client)
			break ;
	}
	if (clients[i] != client)
		return Response::ERR_NOTONCHANNEL(client, &cmd[1][1]);
	if (!it->second.getT() || (it->second.getT() && it->second.isOperator(client)))
	{
		if (cmd.size() < 3 && it->second.getTopic().empty())
			return Response::message(client, "331 " + client->getNick() + " #" + it->second.getName() + " :No topic is set\r\n");
		else if (cmd.size() < 3 && !it->second.getTopic().empty())
			return Response::message(client, "332 " + client->getNick() + " #" + it->second.getName() + " :" + it->second.getTopic() + "\r\n");
		else
		{
			if (cmd[2].at(0) == ':')
				it->second.setTopic(&joinString(cmd, cmd.begin() + 2)[1]);
			else
				it->second.setTopic(joinString(cmd, cmd.begin() + 2));
			Response::message(client, "TOPIC #" + it->second.getName() + " :" + it->second.getTopic() + "\r\n");
			return Response::broadcastChannel(client, &it->second, "TOPIC #" + it->second.getName() + " :" + it->second.getTopic() + "\r\n");
		}
	}
	else
		return Response::ERR_CHANOPRIVSNEEDED(client, "#" + it->second.getName());
}
