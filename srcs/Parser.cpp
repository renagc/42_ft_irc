#include "Parser.hpp"

int Parser::port(char *p)
{
	for (int i = 0; p[i]; i++)
		if (!std::isdigit(p[i]))
			throw ParserPortException();
	return(std::atoi(p));
}

#include "Parser.hpp"

int Parser::port(char *p)
{
	for (int i = 0; p[i]; i++)
		if (!std::isdigit(p[i]))
			throw ParserPortException();
	return(std::atoi(p));
}

int Parser::pass(char *p)
{
	for (int i = 0; p[i]; i++)
		if (p[i] <= '\t' && p[i] >= '\r')
			return 0;
	return 1;
}


std::vector<std::string> Parser::splitString(const std::string &str) {
    
	std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, ' ')) 
	{
		if (token)
        tokens.push_back(token);
    }
    return tokens;
}

int Parser::chooseParsing(std::vector<std::string> tokens) 
{
	switch (tokens[0])
	{
		case "KICK":
			return (KickParser(tokens));
		case "NICKNAME":
			return (nickParser(tokens));
		case "PRIVMESSAGE":
			return (privMessageParser(tokens));
		case "JOIN":
			return (joinParser(tokens));
		case "USERNAME":
			return (userParser(tokens));
		case "INVITE":
			return (inviteParser(tokens));
		case "TOPIC":
			return (topicParser(tokens));
		case "MODE":
			return (nickParser(tokens));
		default:
			return (groupMessageParser(tokens));
	}
	return 0;
}

int Parser::nickParse(std::vector<std::string> tokens) 
{

    if (tokens.size() < 2)
	{
	    std::cout << "Invalid command (more parameters needed)" << std::endl;
	    return 0;
	}
	if (tokens.size() > 2)
	{
	    std::cout << "Invalid command (only 2 parameters required)" << std::endl;
	    return 0;
	}
    if (tokens[1].size() < 3) {
        std::cout << "Invalid command (Nickname need more characters)" << std::endl;
        return 0;
    }
	for (int i = 0; i < tokens[i]; i++)
	{
		if (tokens[i] <= '\t' && tokens[i] >= '\r')
			return 0;
	}
    if (tokens[1].size() > 9 || tokens[1][0] == '#' || tokens[1][0] == ':') 
	{
        std::cout << "Invalid Nickname" << std::endl;
        return 0;
    }
    for (int i = 0; i < ClientsNick.size(); i++) {
        if (ClientsNick.getNickName() == tokens[1]) 
		{
            std::cout << "Repeated Nickname" << std::endl;
            return 0;
        }
    }
    return 1;
}

int Parser::userParse(std::vector<std::string> tokens) {

    std::vector<std::string> command = ?.getCommand();
    if (tokens.size() < 2)
	{
	    std::cout << "Invalid command (more parameters needed)" << std::endl;
	    return 0;
	}
	if (tokens.size() > 2)
	{
	    std::cout << "Invalid command (only 2 parameters required)" << std::endl;
	    return 0;
	}
    if (tokens[1].size() < 3) 
	{
        std::cout << "Invalid command (Nickname need more characters)" << std::endl;
        return 0;
    }
	for (int i = 0; i < tokens[i]; i++)
	{
		if (tokens[i] <= '\t' && str[i] >= '\r')
			return 0;
	}
    if (tokens[1].size() > 9 || tokens[1][0] == '#' || tokens[1][0] == ':') 
	{
        std::cout << "Invalid Nickname" << std::endl;
        return 0;
    }
    for (int i = 0; i < ClientsNick.size(); i++) 
	{
        if (ClientsNick.getNickName() == tokens[1]) 
		{
            std::cout << "Repeated Nickname" << std::endl;
            return 0;
        }
    }
    return 1;
}
int Parser::kickParse(std::vector<std::string> tokens, Client *client, Channel *channel, Server *server) 
{
	std::map<std::string, Channel>::iterator it;
	it = server._channels.find(token[1]);
	if (it != server._channels.end())
	{
		std::vector<Client *> channel_clients;
		
		channel_clients = it->second.getClients();
		for (unsigned long i = 0; i < channel_clients.size(); i++)
		{
			if (channel_clients[i] == token[2])
			{
				std::vector<Client *> operators;
				operators = it->second.getOperators();
				for (unsigned long j = 0; j < operators.size(); j++)
				{
					if (operators[i] == client)
					{
						it->second.removeClient(channel_clients[i])

					}
				}
			}
		}
		command_error;
	}
}

int Parser::InviteParse(std::vector<std::string> tokens, Client *client, Channel *channel, Server *server)
{
	Client	*invited;
	invited = server.getClient(token[1]);
	int	able = 1;
	if (invited != 0)
	{
		std::map<std::string, Channel>::iterator it;
		it = server._channels.find(token[2]);
		if (it != server._channels.end())
		{
			std::vector<Client *> channel_clients;
		
			channel_clients = it->second.getClients();
			for (unsigned long i = 0; i < channel_clients.size(); i++)
			{
				if (channel_clients[i] == invited)
				{
					already in the channel;
				}
			}
			if (it->second.getT() == true)
			{
				able = 0;
				std::vector<Client *> operators;
				operators = it->second.getOperators();
				for (unsigned long j = 0; j < operators.size(); j++)
				{
					if (operators[i] == client)
					able = 1;
				}
			}
			if (it->second.getLimit() < it->second._clients.size() && able == 1)
			{
				it->second.addClient(invited);
			}
			else
			{
				channel full or not operator;
			}
			}
		}
	}

	else
		user doesnt exist


}



int Parser::TopicParse(std::vector<std::string> tokens, Client *client, Channel *channel, Server *server) 
{
	std::map<std::string, Channel>::iterator it;
	it = server._channels.find(token[1]);
	int	able = 1;
	if (it != server._channels.end())
	{
		if (it->second.getT() == true)
		{
			able = 0;
			std::vector<Client *> operators;
			operators = it->second.getOperators();
			for (unsigned long j = 0; j < operators.size(); j++)
			{
				if (operators[i] == client)
					able = 1;
			}
		}
		if (able == 1 && !token[2])
		{
			msg_Topic
		}
		if (able == 1 && token[2])
		{
			setTopic(token[2]);
		}

	}
	else
		channel doesnt exist


}

int Parser::ModeParse(std::vector<std::string> tokens, Client *client, Channel *channel, Server *server) 
{
	std::map<std::string, Channel>::iterator it;
	it = server._channels.find(token[1]);
	if (it != server._channels.end())
	{
		std::vector<Client *> operators;
		operators = it->second.getOperators();
	}
	else
		channel doesnt exist;
	for (unsigned long j = 0; j < operators.size(); j++)
	{
		if (operators[j] == client)
		{
			if (token[2].compare("+i"))
			{
				it->second.setI(true);
			}
			else if (token[2].compare("-i"))
			{
				it->second.setI(false);
			}	
			else if (token[2].compare("+t"))
			{
				it->second.setT(true);
			}
			else if (token[2].compare("-t"))
			{
				it->second.setT(false);
			}
			else if (token[2].compare("+k"))
			{
				it->second.setK(true);
				if (token[3])
					setPW(token[3]);
				else
					setPW(0);
			}
			else if (token[2].compare("-k"))
			{
				if (token[3] && token[3] == it->second._pw)
				{
					it->second.setK(false);
					setPW(0);
				}
				else
					wrong pass;
			}
			else if (token[2].compare("+o"))
			{
				Client *client1;
				client1 = findClient(token[3]);
				for (unsigned long i = 0; i < operators.size(); i++)
				{
					if (operators[j] == client1)
					{
						already operator;
					}
					else
					{
						it->second.addoperator(client1);

					}

				}
			else if (token[2].compare("+l"))
			{
				i->second.setL(true);
				if (token[3])
				{
					for (int j = 0; token[3][j]; j++;)
					{
						if (!isdigit(token[3][j]))
							not a number;
						it->second.setLimit(atoi(token[3]))
					}
				}
				else
					channel doesnt exist
			}
			else if (token[2].compare("-l"))
			{
				it->second.setL(false);
			}
			else
				wrong flag;

			}
		else
			not operator;
		}

}