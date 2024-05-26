#include "Parser.hpp"

// Parser::Parser() {}

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
		return (userCommand(client, cmd[1]));
	else if (!cmd[0].compare("JOIN"))
		return (joinCommand(client, cmd[1]));
	else if (!cmd[0].compare("PRIVMSG"))
		return (privmsgCommand(client, cmd[1], joinString(cmd, cmd.begin() + 2)));
	else if (!cmd[0].compare("KICK"))
		return (kickCommand(client, cmd[1], cmd[2]));
	// else if (cmd[0].compare("INVITE"))
	// 	return (inviteParse(cmd));
	// else if (cmd[0].compare("TOPIC"))
	// 	return (topicParse(cmd));
	// else if (cmd[0].compare("MODE"))
	// 	return (modeParse(cmd));
	else if (!cmd[0].compare("CAP"))
		return ;
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
	throw RPL_WELCOME(client->getNick(), client->getUser(), client->getHost());
}

// atencao ao parsing que esta a dar erro
void Parser::nickCommand( Client *client, const std::string &nickname )
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
	throw RPL_NICK(oldNick, client->getUser(), client->getHost(), nickname);
}

void Parser::userCommand( Client *client, const std::string &username )
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
void Parser::joinCommand( Client *client, const std::string &channel_name )
{
	if (channel_name.empty() || channel_name.at(0) != '#')
	{
		log("no channel name given", client->getId());
		throw ERR_NEEDMOREPARAMS(client->getNick(), "JOIN");
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
	if (client->getLogged())
	{
		channel->add(client);
		log("client joined channel", client->getNick(), channel->getName());
		throw RPL_JOIN(client->getNick(), client->getUser(), client->getHost(), channel->getName());
	}
}

// TODO: verificar o parsing do target
void Parser::privmsgCommand( Client *client, const std::string &channel_name, const std::string &message )
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
		std::string msg = RPL_CHANNEL(client->getNick(), client->getUser(),client->getHost(), &channel_name[1], &message[1]);
		DEBUG(msg);
		for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (client != *it && send((*it)->getFd(), msg.c_str(), msg.size(), 0) == -1)
			{
				log(std::string("send problem"));
			}
		}
		// log(std::string("error channel_name not found"));
		// throw ERR_NOSUCHNICK(client->getNick(), channel_name);
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
void Parser::kickCommand( Client *client, const std::string &channel_name, const std::string &username ) 
{
	std::map<std::string, Channel>::iterator it = _channels->find(channel_name);
	if (it != _channels->end())
		throw "não encontrou channel";
	std::vector<Client *> clients = (*it).second.getClients();
	for (unsigned long i = 0; i < clients.size(); i++)
	{
		if (clients[i]->getUser() == username)
		{
			(*it).second.removeClient(clients[i]);
			throw RPL_KICK(client->getNick(), client->getUser(), channel_name, username);
		}
	}
	throw "client not found";
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
}

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



// int Parser::TopicParse(std::vector<std::string> tokens, Client *client, Channel *channel, Server *server) 
// {
// 	std::map<std::string, Channel>::iterator it;
// 	it = server._channels.find(token[1]);
// 	int	able = 1;
// 	if (it != server._channels.end())
// 	{
// 		if (it->second.getT() == true)
// 		{
// 			able = 0;
// 			std::vector<Client *> operators;
// 			operators = it->second.getOperators();
// 			for (unsigned long j = 0; j < operators.size(); j++)
// 			{
// 				if (operators[i] == client)
// 					able = 1;
// 			}
// 		}
// 		if (able == 1 && !token[2])
// 		{
// 			msg_Topic
// 		}
// 		if (able == 1 && token[2])
// 		{
// 			setTopic(token[2]);
// 		}

// 	}
// 	else
// 		channel doesnt exist


// }

// int Parser::ModeParse(std::vector<std::string> tokens, Client *client, Channel *channel, Server *server) 
// {
// 	std::map<std::string, Channel>::iterator it;
// 	it = server._channels.find(token[1]);
// 	if (it != server._channels.end())
// 	{
// 		std::vector<Client *> operators;
// 		operators = it->second.getOperators();
// 	}
// 	else
// 		channel doesnt exist;
// 	for (unsigned long j = 0; j < operators.size(); j++)
// 	{
// 		if (operators[j] == client)
// 		{
// 			if (token[2].compare("+i"))
// 			{
// 				it->second.setI(true);
// 			}
// 			else if (token[2].compare("-i"))
// 			{
// 				it->second.setI(false);
// 			}	
// 			else if (token[2].compare("+t"))
// 			{
// 				it->second.setT(true);
// 			}
// 			else if (token[2].compare("-t"))
// 			{
// 				it->second.setT(false);
// 			}
// 			else if (token[2].compare("+k"))
// 			{
// 				it->second.setK(true);
// 				if (token[3])
// 					setPW(token[3]);
// 				else
// 					setPW(0);
// 			}
// 			else if (token[2].compare("-k"))
// 			{
// 				if (token[3] && token[3] == it->second._pw)
// 				{
// 					it->second.setK(false);
// 					setPW(0);
// 				}
// 				else
// 					wrong pass;
// 			}
// 			else if (token[2].compare("+o"))
// 			{
// 				Client *client1;
// 				client1 = findClient(token[3]);
// 				for (unsigned long i = 0; i < operators.size(); i++)
// 				{
// 					if (operators[j] == client1)
// 					{
// 						already operator;
// 					}
// 					else
// 					{
// 						it->second.addoperator(client1);

// 					}

// 				}
// 			else if (token[2].compare("+l"))
// 			{
// 				i->second.setL(true);
// 				if (token[3])
// 				{
// 					for (int j = 0; token[3][j]; j++;)
// 					{
// 						if (!isdigit(token[3][j]))
// 							not a number;
// 						it->second.setLimit(atoi(token[3]))
// 					}
// 				}
// 				else
// 					channel doesnt exist
// 			}
// 			else if (token[2].compare("-l"))
// 			{
// 				it->second.setL(false);
// 			}
// 			else
// 				wrong flag;

// 			}
// 		else
// 			not operator;
// 		}

// }