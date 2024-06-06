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
			struct pollfd *pollfds;
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
			if((pollfds = new struct pollfd[50]) == NULL)
			{
				// closefile("new failed", Servers[j].getServer_fd());
				return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
			}
			memset(pollfds, 0, sizeof(pollfds));
			for (int j = 0; j < parser.get_nb_server(); j++)
			{
				if(initialization(Servers[j], 1) != 0)
					return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
				pollfds[j]->fd = serv.getFd();
				pollfds[j]->events = POLLIN;
			}
			// delete []pollfds;
			//Boucle d'écoute//
			while (1)
			{
				for(size_t j = 0; j < parser.get_nb_server(); j++)
				{
					serverConfig serv = Servers[j];
					serv.setNfds(serv.getNumfds());
					//mise a jour de la structure pollfds
					if(poll(serv.getPollfds(), serv.getNfds(), 1000) == -1)
					{
						closefile("poll failed", serv.getFd(), serv.getSocket_client());
						return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
					}
					// on regarde tour à tour les fds ouverts FdSearch()
					for (int fd = 0; fd < serv.getNfds(); fd++)
					{
						//on passe les fd fermés
						if((serv.getPollfds() + fd)->fd <= 0)
							continue;
						//on regarde s'il y a une requeste en attente EventHandling()
						if(((serv.getPollfds() + fd)->revents & POLLIN) == POLLIN)
						{
							cout << "event fd: " << (serv.getPollfds() + fd) -> fd << endl;
							if((serv.getPollfds() + fd) -> fd == serv.getFd())
							{
								// reception du socket du client ClientSocketReception()
								try
								{
									socklen_t len = serv.getAddrlen();
									struct sockaddr *add = serv.getSockaddr();
									serv.setSocket_client(accept(serv.getFd(), add, &len));
									cout << "server fd used for connection: " << serv.getFd() << endl;
									cout << "socket client: " << serv.getSocket_client() << endl;

									if (serv.getSocket_client() < 0)
										throw(std::range_error("accept failed"));
									nblock_connection(serv.getSocket_client(), 1, 2);
									//TODO Should fork() after connection
								}
								catch (exception& e)
								{
									cout << e.what() << endl;
									close(serv.getFd());
									return (1);
								}
								//agrandissement de la structure poll PollExtention()
								if(serv.getNumfds() == serv.getMaxfds())
								{
									serv.setMaxfds(serv.getMaxfds() + 1);
									try
									{
										if((temppollfds = new struct pollfd[serv.getMaxfds()]) == NULL)
											throw(std::range_error("new failed"));
									}
									catch (exception& e)
									{
										cout << e.what() << endl;
										close(serv.getFd());
										close (serv.getSocket_client());
										return (1);
									}
									for(int i = 0; i < serv.getMaxfds() -1; i++)
									{
										temppollfds[i].fd = serv.getPollfds()[i].fd;
										//temppollfds[i].events = serv.getPollfds()[i].events;
										//temppollfds[i].revents = serv.getPollfds()[i].revents;
									}
									if (serv.getPollfds())
									{
										// delete []serv.getPollfds();
										serv.setPollfds(NULL);
									}
									try
									{
										if((pollfds = new struct pollfd[serv.getMaxfds()]) == NULL)
											throw(std::range_error("new failed"));
										serv.setPollfds(pollfds);
									}
									catch (exception& e)
									{
										cout << e.what() << endl;
										close(serv.getFd());
										close (serv.getSocket_client());
										return (1);
									}
									for(int i = 0; i < serv.getNfds(); i++)
									{
										serv.getPollfds()[i].fd = temppollfds[i].fd;
										//serv.getPollfds()[i].events = temppollfds[i].events;
										//serv.getPollfds()[i].revents = temppollfds[i].revents;
									}
									// delete [] temppollfds;
									serv.setMaxfds(serv.getMaxfds() + 5);
								}
								serv.setNumfds(serv.getNumfds() + 1);
								(serv.getPollfds() + serv.getNumfds() -1)->fd = serv.getSocket_client();
								//(serv.getPollfds() + serv.getNumfds() -1)->events = POLLIN;
								//(serv.getPollfds() + serv.getNumfds() -1)->revents = 0;
								//Reception de la requête// RequestReception()
								Request request;
								Response response;
								if (request.RequestReception(serv, serv.getPollfds(), serv.getSocket_client()) == 1)
									continue;
								//TODO extract information from buffer and add it to a struct.
								//Réalisation de la requête//
								std::map<string, string> RequestMap;
								request.createMap(request.getRequestStr(), RequestMap, serv);
								request.RequestExecution(serv, response, text, RequestMap);
								//Creation de la réponse//
								close(serv.getSocket_client());
								std::cout << "fin de la requ[e]te" << std::endl;
							}
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
// 				Servers[j].setPollfds(pollfds);
// 				// delete []pollfds;
// 				//Initialization//
// 				if(initialization(Servers[j], 1, Servers[j].getPollfds()) != 0)
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
// 					if(poll(serv.getPollfds(), serv.getNfds(), 1000) == -1)
// 					{
// 						closefile("poll failed", serv.getFd(), serv.getSocket_client());
// 						return(INTERNAL_SERVER_ERROR); //TODO ajouter page d'erreur
// 					}
// 					// on regarde tour à tour les fds ouverts FdSearch()
// 					for (int fd = 0; fd < serv.getNfds(); fd++)
// 					{
// 						//on passe les fd fermés
// 						if((serv.getPollfds() + fd)->fd <= 0)
// 							continue;
// 						//on regarde s'il y a une requeste en attente EventHandling()
// 						if(((serv.getPollfds() + fd)->revents & POLLIN) == POLLIN)
// 						{
// 							cout << "event fd: " << (serv.getPollfds() + fd) -> fd << endl;
// 							if((serv.getPollfds() + fd) -> fd == serv.getFd())
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
// 										temppollfds[i].fd = serv.getPollfds()[i].fd;
// 										//temppollfds[i].events = serv.getPollfds()[i].events;
// 										//temppollfds[i].revents = serv.getPollfds()[i].revents;
// 									}
// 									if (serv.getPollfds())
// 									{
// 										// delete []serv.getPollfds();
// 										serv.setPollfds(NULL);
// 									}
// 									try
// 									{
// 										if((pollfds = new struct pollfd[serv.getMaxfds()]) == NULL)
// 											throw(std::range_error("new failed"));
// 										serv.setPollfds(pollfds);
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
// 										serv.getPollfds()[i].fd = temppollfds[i].fd;
// 										//serv.getPollfds()[i].events = temppollfds[i].events;
// 										//serv.getPollfds()[i].revents = temppollfds[i].revents;
// 									}
// 									// delete [] temppollfds;
// 									serv.setMaxfds(serv.getMaxfds() + 5);
// 								}
// 								serv.setNumfds(serv.getNumfds() + 1);
// 								(serv.getPollfds() + serv.getNumfds() -1)->fd = serv.getSocket_client();
// 								//(serv.getPollfds() + serv.getNumfds() -1)->events = POLLIN;
// 								//(serv.getPollfds() + serv.getNumfds() -1)->revents = 0;
// 								//Reception de la requête// RequestReception()
// 								Request request;
// 								Response response;
// 								if (request.RequestReception(serv, serv.getPollfds(), serv.getSocket_client()) == 1)
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


