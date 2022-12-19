// Example code: A simple server side code, which echos back the received message.
// Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>	   //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include <pthread.h>

#include "config.h"
#include "advanced_config.h"
#include "compile_modes.h"

#include "global_vars.h"

#include "thread.h"

#define TRUE 1
#define FALSE 0

int client_socket[MAX_CONNECTIONS];
int max_clients = MAX_CONNECTIONS;
pthread_t thread_file_descriptors[MAX_CONNECTIONS];


struct thread_data client_specific_thread_payload[MAX_CONNECTIONS];


// master thread code came from: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
int main(int argc, char *argv[])
{
	int opt = TRUE;
	int master_socket, addrlen, new_socket,
		activity, i, valread, sd;
	int max_sd;
	struct sockaddr_in address;

	// set of socket descriptors
	fd_set readfds;

	// a message
	char *errorMessages[1] = {
		"Max number of concurrent users has been reached, try again later."};
	enum errorMessageid
	{
		none = -1,
		tooManyConcurrentConnections = 0
	};

	// initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	// create a master socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// set master socket to allow multiple connections ,
	// this is just a good habit, it will work without this
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
				   sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);

	// try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	// accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");

	while (TRUE)
	{
		enum errorMessageid error;
		// clear the socket set
		FD_ZERO(&readfds);

		// add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		// add child sockets to set
		for (i = 0; i < max_clients; i++)
		{
			// socket descriptor
			sd = client_socket[i];

			// if valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);

			// highest file descriptor number, need it for the select function
			if (sd > max_sd)
				max_sd = sd;
		}

		// wait for an activity on one of the sockets , timeout is NULL ,
		// so wait indefinitely
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR))
		{
			printf("select error");
		}

		// If something happened on the master socket ,
		// then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
									 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			// inform user of socket number - used in send and receive commands
			#ifdef DEBUG_COMPILE_MODE
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
			#endif

			int8_t found_slot = 0;
			for (i = 0; i < max_clients; i++)
			{
				if (thread_file_descriptors[i] == 0)
				{
					// found an available slot and will reserve it and set the data to be sent to the thread
					found_slot += 1;
					client_specific_thread_payload[i].client_socket_file_descriptor = new_socket;
					client_specific_thread_payload[i].current_thread_file_descriptor_id = i;
					pthread_create(&thread_file_descriptors[i], NULL, handleRequest, (void *)&client_specific_thread_payload[i]);
					break;
				}
			}
			if (found_slot == 0)
			{
				error = tooManyConcurrentConnections;
				send(new_socket, errorMessages[error], strlen(errorMessages[error]), 0);
				#ifdef DEBUG_COMPILE_MODE
				printf("too many connections. aborting.\n");
				#endif
				close(new_socket);
			}
		}
	}

	return 0;
}
