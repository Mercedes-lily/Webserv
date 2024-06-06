#include "../inc/serverConfig.hpp"
#include "../inc/Request.hpp"
#include "../inc/Response.hpp"
#include "../inc/errorCode.hpp"
#include "../inc/initialization.hpp"
#include "../inc/webserv.hpp"
#include "../inc/configParser.hpp"
#include <sstream>
#include <cstdlib>
using std::string;
using std::exception;
using std::cout;
using std::endl;
using std::remove;

int	main(int argc, char **argv)
{
	if (argc == 1 || argc == 2)
	{
		try
		{
			std::map<int, int> SocketsList;
			int nbActiveSockets = 0;
			std::string config;
			configParser parser;
			if (argc == 1)
				config = "config/default.conf";
			else
				config = argv[1];
			parser.initializeServerConfigurations(config);
			// parser.print();

			//TODO get config and set server or set with default values
			string text;
			std::vector<serverConfig> &Servers = parser.getServers();
			//Server creation//
			//Initialization//
			struct pollfd *pollfds = new struct pollfd[50];
			if(pollfds == NULL)
			{
				// closefile("new failed", Servers[j].getServer_fd());
				return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
			}
			memset(pollfds, 0, sizeof(struct pollfd[50]));
			for (size_t j = 0; j < parser.get_nb_server(); j++)
			{
				if(initialization(Servers[j], 1) != 0)
					return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
				pollfds[j].fd = Servers[j].getFd();
				pollfds[j].events = POLLIN;
				SocketsList[pollfds[j].fd] = pollfds[j].fd;
				cout << "pollfds[j].fd]: " << pollfds[j].fd << endl;
				nbActiveSockets++;
			}
			// delete []pollfds;
			//Boucle d'écoute//
			while (1)
			{
				//mise à jour de la structure pollfds
				if(poll(pollfds, nbActiveSockets, 1000) == -1)
				{
					//closefile("poll failed", serv.getFd(), serv.getSocket_client());
					return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
				}
				// on regarde tour à tour les fds ouverts FdSearch()
				for (int p = 0; p < nbActiveSockets; p++)
				{
					//on passe les fd fermés
					if((pollfds[p]).fd <= 0)
						continue;
					//on regarde s'il y a une requeste en attente EventHandling()
					if((pollfds[p].revents & POLLIN) == POLLIN)
					{
						cout << "event fd: " << pollfds[p].fd << endl;
						cout << "p: " << p << endl;
						cout << "SocketsList[pollfds[p].fd]: " << SocketsList[pollfds[p].fd] << endl;
						if(pollfds[p].fd == SocketsList[pollfds[p].fd])  //on dirait que cette vérification implique que l'on n'a besoin d'écouter que les sockets serveurs.
						{
							// reception du socket du client ClientSocketReception()
							int ActuelServerId = pollfds[p].fd - 3;
							socklen_t len = Servers[ActuelServerId].getAddrlen();
							struct sockaddr *add = Servers[ActuelServerId].getSockaddr();
							Servers[ActuelServerId].setSocket_client(accept(Servers[ActuelServerId].getFd(), add, &len));
							cout << "server fd used for connection: " << Servers[ActuelServerId].getFd() << endl;
							cout << "socket client: " << Servers[ActuelServerId].getSocket_client() << endl;
							SocketsList[Servers[ActuelServerId].getSocket_client()] = pollfds[p].fd;
							nbActiveSockets++;
							if (Servers[ActuelServerId].getSocket_client() < 0)
							{
								cout << "accept failed" << endl;
								close(Servers[ActuelServerId].getFd());
								return (1);
							}
							nblock_connection(Servers[ActuelServerId].getSocket_client(), 1, 2);
							//Reception de la requête// RequestReception()
							Request request;
							Response response;
							if (request.RequestReception(Servers[ActuelServerId], pollfds, Servers[ActuelServerId].getSocket_client()) == 1)
								continue;
							//TODO extract information from buffer and add it to a struct.
							//Réalisation de la requête//
							std::map<string, string> RequestMap;
							request.createMap(request.getRequestStr(), RequestMap, Servers[ActuelServerId]);
							request.RequestExecution(Servers[ActuelServerId], response, text, RequestMap);
							//Creation de la réponse//
							close(Servers[ActuelServerId].getSocket_client());
							std::cout << "fin de la requ[e]te" << std::endl;
						}
					}
				}
			}
			for(size_t j = 0; j < parser.get_nb_server(); j++)
			{
				close(Servers[j].getSocket_client());
			}
			return (0);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return 1;
		}
	}
	else
	{
		std::cerr << RED << "Usage: ./webserv [config_file]" << RESET << std::endl;
		return 1;
	}
	return 0;
}

//TODO create html file
//TODO learn to use nginx




// int	main(int argc, char **argv)
// {
// 	if (argc == 1 || argc == 2)
// 	{
// 		try
// 		{
// 			std::string config;
// 			configParser parser;
// 			if (argc == 1)
// 				config = "config/default.conf";
// 			else
// 				config = argv[1];
// 			parser.initializeServerConfigurations(config);
// 			// parser.print();

// 			//TODO get config and set server or set with default values
// 			struct pollfd *pollfds = NULL;
// 			struct pollfd *temppollfds;
// 			string text;
// 			std::vector<serverConfig> &Servers = parser.getServers();
// 			//Server creation//
// 			for(size_t j = 0; j < parser.get_nb_server(); j++)
// 			{
// 				if((pollfds = new struct pollfd[Servers[j].getMaxfds()]) == NULL)
// 				{
// 					closefile("new failed", Servers[j].getServer_fd());
// 					return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
// 				}
// 				Servers[j].setpollfds(pollfds);
// 				// delete []pollfds;
// 				//Initialization//
// 				if(initialization(Servers[j], 1, Servers[j].getpollfds()) != 0)
// 					return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
// 			}
// 			//Boucle d'écoute//
// 			while (1)
// 			{
// 				for(size_t j = 0; j < parser.get_nb_server(); j++)
// 				{
// 					serverConfig serv = Servers[j];
// 					serv.setNfds(serv.getNumfds());
// 					//mise a jour de la structure pollfds
// 					if(poll(serv.getpollfds(), serv.getNfds(), 1000) == -1)
// 					{
// 						closefile("poll failed", serv.getFd(), serv.getSocket_client());
// 						return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
// 					}
// 					// on regarde tour à tour les fds ouverts FdSearch()
// 					for (int fd = 0; fd < serv.getNfds(); fd++)
// 					{
// 						//on passe les fd fermés
// 						if((serv.getpollfds() + fd)->fd <= 0)
// 							continue;
// 						//on regarde s'il y a une requeste en attente EventHandling()
// 						if(((serv.getpollfds() + fd)->revents & POLLIN) == POLLIN)
// 						{
// 							cout << "event fd: " << (serv.getpollfds() + fd) -> fd << endl;
// 							if((serv.getpollfds() + fd) -> fd == serv.getFd())
// 							{
// 								// reception du socket du client ClientSocketReception()
// 								try
// 								{
// 									socklen_t len = serv.getAddrlen();
// 									struct sockaddr *add = serv.getSockaddr();
// 									serv.setSocket_client(accept(serv.getFd(), add, &len));
// 									cout << "server fd used for connection: " << serv.getFd() << endl;
// 									cout << "socket client: " << serv.getSocket_client() << endl;

// 									if (serv.getSocket_client() < 0)
// 										throw(std::range_error("accept failed"));
// 									nblock_connection(serv.getSocket_client(), 1, 2);
// 									//TODO Should fork() after connection
// 								}
// 								catch (exception& e)
// 								{
// 									cout << e.what() << endl;
// 									close(serv.getFd());
// 									return (1);
// 								}
// 								//agrandissement de la structure poll PollExtention()
// 								if(serv.getNumfds() == serv.getMaxfds())
// 								{
// 									serv.setMaxfds(serv.getMaxfds() + 1);
// 									try
// 									{
// 										if((temppollfds = new struct pollfd[serv.getMaxfds()]) == NULL)
// 											throw(std::range_error("new failed"));
// 									}
// 									catch (exception& e)
// 									{
// 										cout << e.what() << endl;
// 										close(serv.getFd());
// 										close (serv.getSocket_client());
// 										return (1);
// 									}
// 									for(int i = 0; i < serv.getMaxfds() -1; i++)
// 									{
// 										temppollfds[i].fd = serv.getpollfds()[i].fd;
// 										//temppollfds[i].events = serv.getpollfds()[i].events;
// 										//temppollfds[i].revents = serv.getpollfds()[i].revents;
// 									}
// 									if (serv.getpollfds())
// 									{
// 										// delete []serv.getpollfds();
// 										serv.setpollfds(NULL);
// 									}
// 									try
// 									{
// 										if((pollfds = new struct pollfd[serv.getMaxfds()]) == NULL)
// 											throw(std::range_error("new failed"));
// 										serv.setpollfds(pollfds);
// 									}
// 									catch (exception& e)
// 									{
// 										cout << e.what() << endl;
// 										close(serv.getFd());
// 										close (serv.getSocket_client());
// 										return (1);
// 									}
// 									for(int i = 0; i < serv.getNfds(); i++)
// 									{
// 										serv.getpollfds()[i].fd = temppollfds[i].fd;
// 										//serv.getpollfds()[i].events = temppollfds[i].events;
// 										//serv.getpollfds()[i].revents = temppollfds[i].revents;
// 									}
// 									// delete [] temppollfds;
// 									serv.setMaxfds(serv.getMaxfds() + 5);
// 								}
// 								serv.setNumfds(serv.getNumfds() + 1);
// 								(serv.getpollfds() + serv.getNumfds() -1)->fd = serv.getSocket_client();
// 								//(serv.getpollfds() + serv.getNumfds() -1)->events = POLLIN;
// 								//(serv.getpollfds() + serv.getNumfds() -1)->revents = 0;
// 								//Reception de la requête// RequestReception()
// 								Request request;
// 								Response response;
// 								if (request.RequestReception(serv, serv.getpollfds(), serv.getSocket_client()) == 1)
// 									continue;
// 								//TODO extract information from buffer and add it to a struct.
// 								//Réalisation de la requête//
// 								std::map<string, string> RequestMap;
// 								request.createMap(request.getRequestStr(), RequestMap, serv);
// 								request.RequestExecution(serv, response, text, RequestMap);
// 								//Creation de la réponse//
// 								close(serv.getSocket_client());
// 								std::cout << "fin de la requ[e]te" << std::endl;
// 							}
// 						}
// 					}
// 				}
// 			}
// 			for(size_t j = 0; j < parser.get_nb_server(); j++)
// 			{
// 				close(Servers[j].getSocket_client());
// 			}
// 			return (0);
// 		}
// 		catch (const std::exception& e)
// 		{
// 			std::cerr << e.what() << '\n';
// 			return 1;
// 		}
// 	}
// 	else
// 	{
// 		std::cerr << RED << "Usage: ./webserv [config_file]" << RESET << std::endl;
// 		return 1;
// 	}
// 	return 0;
// }

// //TODO create html file
// //TODO learn to use nginx


