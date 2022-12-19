#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H
#include <pthread.h>

#include "config.h"
#include "advanced_config.h"
#include "compile_modes.h"

extern int client_socket[MAX_CONNECTIONS];
extern pthread_t thread_file_descriptors[MAX_CONNECTIONS];
struct thread_data
{
	int client_socket_file_descriptor;
	pthread_t current_thread_file_descriptor_id;
};
extern struct thread_data client_specific_thread_payload[MAX_CONNECTIONS];
#endif //GLOBAL_VARS_H