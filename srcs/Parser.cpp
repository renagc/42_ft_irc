#include "Parser.hpp"

Parser::Parser(Server *server) : _server(server)
{
	_clients = &server->getClients();
	_channels = &server->getChannels();
}

Parser::~Parser() {}

void Parser::chooseParsing(Client *client, std::vector<std::string> cmd)
{
	if (cmd.size() < 1)
		throw "Invalid command (more parameters needed)";
	if (!cmd[0].compare("NICK"))
		return (nickCommand(client, cmd[1]));
	else if (!cmd[0].compare("USER"))
		return (userCommand(client, cmd[1]));
	else if (client->getLogged())
	{
		if (!cmd[0].compare("JOIN"))
			return (joinCommand(client, cmd[1]));
		else if (!cmd[0].compare("PRIVMSG"))
			return (privmsgCommand(client, cmd[1], joinString(cmd, cmd.begin() + 2)));
		else if (!cmd[0].compare("KICK"))
			return (kickCommand(client, cmd))
		// else if (cmd[0].compare("INVITE"))
		// 	return (inviteParse(cmd));
		// else if (cmd[0].compare("TOPIC"))
		// 	return (topicParse(cmd));
		// else if (cmd[0].compare("MODE"))
		// 	return (modeParse(cmd));
		else if (!cmd[0].compare("CAP"))
			return;
	}
	else
		throw ERR_UNKNOWNCOMMAND(client->getNick(), cmd[0]);
}

std::map<int, Client>::iterator Parser::findNick(const std::string &nick)
{
	std::map<int, Client>::iterator it;
	for (it = _clients->begin(); it != _clients->end(); it++)
	{
		if (it->second.getNick() == nick)
			break;
	}
	return (it);
}

void Parser::authenticateChecker(Client *client)
{
	if (client->getNick().empty() || client->getUser().empty())
		return;
	client->setLogged(true);
	throw RPL_WELCOME(client->getNick(), client->getUser(), client->getHost());
}

// atencao ao parsing que esta a dar erro
void Parser::nickCommand(Client *client, const std::string &nickname)
{
	if (nickname.empty())
	{
		log(std::string("no nickname given"));
		throw ERR_NONICKNAMEGIVEN(client->getNick());
	}
	std::map<int, Client>::iterator it = findNick(nickname);
	if (it != _clients->end())
	{
		log(std::string("error nickname in use"));
		throw ERR_NICKNAMEINUSE(client->getNick(), nickname);
	}
	log(std::string("client changed nickname to: ").append(nickname), client->getId());
	std::string oldNick = client->getNick();
	client->setNick(nickname);
	if (!client->getLogged())
		authenticateChecker(client);
	else
		throw RPL_NICK(oldNick, client->getUser(), client->getHost(), nickname);
}

void Parser::userCommand(Client *client, const std::string &username)
{
	if (username.empty())
	{
		log(std::string("no username or realname given"));
		throw ERR_NEEDMOREPARAMS(client->getNick(), "USER");
	}
	client->setUser(username);
	log(std::string("client changed username to: ").append(username), client->getId());
	if (!client->getLogged())
		authenticateChecker(client);
}

// JOIN #channel need to implement all the error messages
void Parser::joinCommand(Client *client, const std::string &channel_name)
{
	if (channel_name.empty() || channel_name.at(0) != '#')
	{
		log("no channel name given", client->getId());
		throw ERR_NEEDMOREPARAMS(client->getNick(), "JOIN");
	}
	Channel *channel = NULL;
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
void Parser::privmsgCommand(Client *client, const std::string &channel_name, const std::string &message)
{
	log(message);
	if (channel_name.empty() || message.empty())
	{
		log(std::string("no channel_name or message given"));
		throw ERR_NEEDMOREPARAMS(client->getNick(), "PRIVMSG");
	}
	std::map<std::string, Channel>::iterator it = _channels->find(&channel_name[1]);
	if (it != _channels->end())
	{
		std::vector<Client *> clients = (*it).second.getClients();
		std::string msg = RPL_CHANNEL(client->getNick(), client->getUser(), client->getHost(), &channel_name[1], &message[1]);
		for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (client->getFd() != (*it)->getFd() && send((*it)->getFd(), msg.c_str(), msg.size(), 0) == -1)
				log(std::string("send problem"));
		}
	}
	log(std::string("client sent message to ").append(channel_name), client->getId());
	// throw RPL_PRIVMSG(client->getNick(), client->getUser(), channel_name, message);
}

// int Parser::nickParse(std::vector<std::string> tokens)
// {

//     if (tokens.size() < 2)
// 	{
// 	    std::cout << "Invalid command (more parameters needed)" << std::endl;
// 	    return 0;
// 	}
// 	if (tokens.size() > 2)
// 	{
// 	    std::cout << "Invalid command (only 2 parameters required)" << std::endl;
// 	    return 0;
// 	}
//     if (tokens[1].size() < 3) {
//         std::cout << "Invalid command (Nickname need more characters)" << std::endl;
//         return 0;
//     }
// 	for (int i = 0; i < tokens[i]; i++)
// 	{
// 		if (tokens[i] <= '\t' && tokens[i] >= '\r')
// 			return 0;
// 	}
//     if (tokens[1].size() > 9 || tokens[1][0] == '#' || tokens[1][0] == ':')
// 	{
//         std::cout << "Invalid Nickname" << std::endl;
//         return 0;
//     }
//     for (int i = 0; i < ClientsNick.size(); i++) {
//         if (ClientsNick.getNickName() == tokens[1])
// 		{
//             std::cout << "Repeated Nickname" << std::endl;
//             return 0;
//         }
//     }
//     return 1;
// }

// int Parser::userParse(std::vector<std::string> tokens) {

//     std::vector<std::string> command = ?.getCommand();
//     if (tokens.size() < 2)
// 	{
// 	    std::cout << "Invalid command (more parameters needed)" << std::endl;
// 	    return 0;
// 	}
// 	if (tokens.size() > 2)
// 	{
// 	    std::cout << "Invalid command (only 2 parameters required)" << std::endl;
// 	    return 0;
// 	}
//     if (tokens[1].size() < 3)
// 	{
//         std::cout << "Invalid command (Nickname need more characters)" << std::endl;
//         return 0;
//     }
// 	for (int i = 0; i < tokens[i]; i++)
// 	{
// 		if (tokens[i] <= '\t' && str[i] >= '\r')
// 			return 0;
// 	}
//     if (tokens[1].size() > 9 || tokens[1][0] == '#' || tokens[1][0] == ':')
// 	{
//         std::cout << "Invalid Nickname" << std::endl;
//         return 0;
//     }
//     for (int i = 0; i < ClientsNick.size(); i++)
// 	{
//         if (ClientsNick.getNickName() == tokens[1])
// 		{
//             std::cout << "Repeated Nickname" << std::endl;
//             return 0;
//         }
//     }
//     return 1;
// }

// TODO: adicionar parsing do comentario como argumento ou nao e verificar o mode do channel
void Parser::kickCommand(Client *client, const std::vector<std::string> cmd)
{
	if (cmd[1].empty() || cmd[2].empty()) // parametros insuficientes
		ERR_NEEDMOREPARAMS;
	if (cmd[1][0] != '#') // sem #
		ERR_NOSUCHCHANNEL;
	std::map<std::string, Channel>::iterator it = _channels->find(&cmd[1][1]);
	if (it == _channels->end()) // channel nao existe
		ERR_NOSUCHCHANNEL;
	std::vector<Client *> clientsOnChannel = it->second.getClients();
	std::vector<Client *> operatorsOnChannel = it->second.getOperators();
	for (unsigned long i = 0; i < clientsOnChannel.size(); i++)
	{
		if (clientsOnChannel[i] ==  client)
		{
			if (!it->second.isOperator(client)) // nao e operador
				ERR_CHANOPRIVSNEEDED;
			for (unsigned long j = 0; j < clientsOnChannel.size(); j++)
			{
				if (!clientsOnChannel[j]->getUser().compare(cmd[2]))
				{
					(*it).second.removeClient(clientsOnChannel[j]);
					throw RPL_KICK(client->getNick(), client->getUser(), channel_name, username);
				}
			}
			ERR_USERNOTINCHANNEL; // client que seria kickado


		}
	}
	ERR_USERNOTINCHANNEL; // client que tenta kickar


}
	// std::map<std::string, Channel>::iterator it;
	// it = _server->_channels.find(token[1]);
	// if (it != server._channels.end())
	// {
	// 	std::map<int, Client> channel_clients;

	// 	channel_clients = it->second.getClients();
	// 	for (unsigned long i = 0; i < channel_clients.size(); i++)
	// 	{
	// 		if (channel_clients[i] == cmd[2])
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
}

void Parser::InviteParse(Client *client, const std::vector<std::string> cmd)
 {
	if (cmd[1].empty() || cmd[2].empty()) // parametros insuficientes
		ERR_NEEDMOREPARAMS;
	if (cmd[2][0] != '#') // sem #
		ERR_NOSUCHCHANNEL;
	std::map<std::string, Channel>::iterator it = _channels->find(&cmd[2][1]);
	if (it == _channels->end()) // channel nao existe
		ERR_NOSUCHCHANNEL;
	std::vector<Client *> clientsOnChannel = it->second.getClients();
	std::vector<Client *> operatorsOnChannel = it->second.getOperators();
	for (unsigned long i = 0; i < clientsOnChannel.size(); i++)
	{
		if (clientsOnChannel[i] ==  client)
		{
			if (!it->second.isOperator(client)) // nao e operador
				ERR_CHANOPRIVSNEEDED;
			for (unsigned long j = 0; j < clientsOnChannel.size(); j++)
			{
				if (!clientsOnChannel[j]->getNick().compare(cmd[1])) // user ja esta no channel
					throw ERR_USERONCHANNEL;
			}
			if ((it->second.getL() == true && it->second._limit < it->second._clients.size()) || it->second.getL() == false)
			{
				if (_server.getClient(cmd1) != NULL)
				{
					it->second.add(_server.getClient(cmd1));
					RPL_INVITING();

				}
				else // nickname nao existe
					ERR_NOSUCHNICK;
			}

		}
	}
	ERR_USERNOTINCHANNEL; // client que convida nao esta no channel
 }

 int Parser::TopicParse(Client *client, const std::vector<std::string> cmd)
 {
	if (cmd[1].empty()) // parametros insuficientes
		ERR_NEEDMOREPARAMS;
	if (cmd[1][0] != '#') // sem #
		ERR_NOSUCHCHANNEL;
	std::map<std::string, Channel>::iterator it = _channels->find(&cmd[1][1]);
	if (it == _channels->end()) // channel nao existe
		ERR_NOSUCHCHANNEL;
	for (unsigned long i = 0; i < clientsOnChannel.size(); i++)
	{
		if (clientsOnChannel[i] ==  client)
		{
			if (it->second.getT() == false || (it->second.getT() == true && it->second.isOperator(client)))
 			{
				if (!cmd[2])
					RPL_NOTOPIC;
				else
				{
					std::string topic = cmd[2];
					for (int j = 3; j < cmd.size(); j++)
					{
						topic.append(" ");
						topic.append(cmd[j]);
						it->second.setTopic(topic); // criar funcao setTopic
					}
					RPL_TOPIC;

				}

			}
			else // nao e operador e _t = true
				ERR_CHANOPRIVSNEEDED;
		}
	}
}
 int Parser::ModeParse(std::vector<std::string> tokens, Client *client, Channel *channel, Server *server)
 {
	if (cmd[1].empty()) // parametros insuficientes
		ERR_NEEDMOREPARAMS;
	if (cmd[1][0] != '#') // sem #
		ERR_NOSUCHCHANNEL;
	if (cmd[2])
		ERR_UMODEUNKNOWNFLAG;
	std::map<std::string, Channel>::iterator it = _channels->find(&cmd[1][1]);
	if (it == _channels->end()) // channel nao existe
		ERR_NOSUCHCHANNEL;
	std::vector<Client *> clientsOnChannel = it->second.getClients();
	std::vector<Client *> operatorsOnChannel = it->second.getOperators();
	for (unsigned long i = 0; i < clientsOnChannel.size(); i++)
	{
		if (clientsOnChannel[i] ==  client)
		{
			if (!it->second.isOperator(client)) // nao e operador
				ERR_CHANOPRIVSNEEDED;
			else
			{
				if (cmd[2].compare("+i"))
				{
					it->second.setI(true);
				}
				else if (cmd[2].compare("-i"))
				{
					it->second.setI(false);
				}
				else if (cmd[2].compare("+t"))
				{
					it->second.setT(true);
				}
				else if (cmd[2].compare("-t"))
				{
					it->second.setT(false);
				}
				else if (cmd[2].compare("+k"))
				{
					it->second.setK(true);
					if (cmd[3])
						setPW(cmd[3]);
					else
						setPW(NULL);
				}
				else if (!cmd[2].compare("-k"))
				{
					if (cmd[3] && !cmd[3].compare(it->second._pw))
					{
						it->second.setK(false);
						setPW(NULL);
					}
					else
						ERR_KEYSET;
				}
				else if (!cmd[2].compare("+o"))
				{
					Client *client1;
					client1 = findClient(cmd[3]);
					for (unsigned long q = 0; q < operators.size(); q++)
					{
						if (operators[q] == client1)
						{
							return (0);
						}
						else
						{
							it->second.addoperator(client1);
						}
					}
				else if (!cmd[2].compare("+l"))
				{
					i->second.setL(true);
					if (cmd[3])
					{
						for (int j = 0; cmd[3][j]; j++;)
						{
							if (!isdigit(cmd[3][j]))
								ERR_NEEDMOREPARAMS;
							it->second.setLimit(atoi(cmd[3]))
						}
					}
					else
						ERR_NEEDMOREPARAMS;
				}
				else if (!cmd[2].compare("-l"))
				{
					it->second.setL(false);
				}
				else
					ERR_UMODEUNKNOWNFLAG;
				}
			}
		}
	}
}


