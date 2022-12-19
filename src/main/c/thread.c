#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "config.h"
#include "advanced_config.h"
#include "compile_modes.h"

#include "global_vars.h"
#include "thread.h"

#include "sql.h"

//opens a file for reading, and buffers the contents
char* openAndBufferFile(char* path){
	FILE* filePointer = fopen(path, "r");
	//buffer and read the entire file
	fseek(filePointer, 0, SEEK_END);
	long fileSize = ftell(filePointer);
	fseek(filePointer, 0, SEEK_SET);
	char* outputFile = malloc(fileSize+1);
	fread(outputFile, fileSize, 1, filePointer);
	fclose(filePointer);
	outputFile[fileSize] = 0;
	return outputFile;
}

void sendFileToSocket(int socket_fd, int file_fd, int file_size){
	sendfile(socket_fd, file_fd, NULL, file_size);
}

void sendStringToSocket(int socket_fd, char* payload){}

void *handleRequest(void *client_specific_payload)
{
	int errorOccured = 0;
	char* errorMessage = NULL;

	// make sure the argument goes from a void to a structure so we can access the data
	struct thread_data *formatted_payload = (struct thread_data *)client_specific_payload;

	int client_socket_fd = formatted_payload->client_socket_file_descriptor;
	pthread_t *activeThread = &thread_file_descriptors[formatted_payload->current_thread_file_descriptor_id];

	char buffer[MAX_BUFFER] = {0}; // data buffer set to MAX_BUFFER in config.h

	int valread = read(client_socket_fd, buffer, MAX_BUFFER);

	if (valread == 0)
	{
		close(client_socket_fd);
	}

	char *message = "HTTP/1.0 200 OK\n";


	// stateless operation of getting header new lines (only thing stored is the start of the new line)
	char *headers[MAX_HEADERS];
	int startOfHeaderLine = 0;
	int currentNumberOfHeaders = 0;

	char *header_token = strtok((char *)buffer, "\n");

	int i = 0;
	while (header_token != NULL)
	{
		if(i>=MAX_HEADERS){ //prevent too many headers from being saved

			#ifdef DEBUG_COMPILE_MODE
			printf("Too many headers, stopping execution now\n");
			#endif

			header_token = NULL;
			break;
		}
		//make an array of headers
		headers[i] = header_token;

		#ifdef DEBUG_COMPILE_MODE
		printf("%d: %s\n", i, header_token);
		#endif

		header_token = strtok(NULL, "\n");
		i++;
	}

	//find out which type of request it is, the human-readable codes are in config.h
	char *requestTypes[NUM_HTTP_CODES] = HTTP_CODES_ARRAY;
	int currentRequestId = NUM_HTTP_CODES;
	for( int j = 0; j < NUM_HTTP_CODES; j++){
		if(strncmp(headers[0], requestTypes[j], strlen(requestTypes[j]))==0){
			currentRequestId = j;
			break;
		}
	}

	#ifdef DEBUG_COMPILE_MODE
	printf("request ID: %d\n", currentRequestId);
	#endif

	char* finalMessage;
	long finalLength;

	if(currentRequestId == 0){
		char* filePathStart = strchr(headers[0], ' ');
		char* filePathEnd = strchr(filePathStart+1, ' ');
		int filePathLength = filePathEnd-filePathStart-1;
		char* file = NULL;
		file = malloc(filePathLength+2);
		memset(file, 0, filePathLength+1);
		strncpy(file, filePathStart+1, filePathLength);
		char *impliedFile;

		#ifdef DEBUG_COMPILE_MODE
		printf("%s\n", file);
		#endif

		if((char)file[strlen(file)-1] == '/'){
			impliedFile = "index.html";
		} else {
			impliedFile = "";
		}
		int pathlen = strlen(DATA_DIRECTORY)+strlen(file)+strlen(impliedFile); //get the full path of the file
		char* path = malloc(pathlen+1);
		memset(path, 0, pathlen);
		path[pathlen] = 0;
		strcpy(path, DATA_DIRECTORY);
		strcat(path, file);
		strcat(path, impliedFile);

		#ifdef DEBUG_COMPILE_MODE
		printf("full path: %s\n", path);
		#endif

		char* headerPath = malloc(pathlen+9);
		strcpy(headerPath, path);
		strcat(headerPath, ".headers");
		if((access(path, F_OK) == 0) && (access(headerPath, F_OK) == 0)){
			//file exists
			#ifdef DEBUG_COMPILE_MODE
			printf("file exists\n");
			#endif

			FILE* staticFile = fopen(path, "r");
			FILE* staticHeaders = fopen(headerPath, "r");

			//get the HTML file
			fseek(staticFile, 0, SEEK_END);
			long fileSize = ftell(staticFile);
			fseek(staticFile, 0, SEEK_SET);
			char* outputFile = malloc(fileSize+2);
			fread(outputFile, fileSize, 1, staticFile);
			fclose(staticFile);
			outputFile[fileSize] = 0;

			//get the test file of headers
			fseek(staticHeaders, 0, SEEK_END);
			long headerFileSize = ftell(staticHeaders);
			fseek(staticHeaders, 0, SEEK_SET);
			char* outputHeader = malloc(headerFileSize+1);
			fread(outputHeader, headerFileSize, 1, staticHeaders);
			fclose(staticHeaders);
			outputHeader[headerFileSize] = 0;

			//prepare final message
			finalLength = fileSize + headerFileSize + strlen(message)+2;
			finalMessage = malloc(finalLength+1);
			finalMessage[finalLength] = 0;
			memset(finalMessage, 0, finalLength);
			strcpy(finalMessage, message);
			strcat(finalMessage, outputHeader);
			strcat(finalMessage, "\n\n");
			strcat(finalMessage, outputFile);

			free(outputFile);
			free(outputHeader);

			// #ifdef DEBUG_COMPILE_MODE
			// printf("%s\n", finalMessage);
			// #endif
		} else { //file does not exist

			#ifdef DEBUG_COMPILE_MODE
			printf("pathlen: %d\n", strlen(path));
			#endif
			if(file[strlen(file)-1] != '/'){
				char movedError[] = "HTTP/1.1 301 Moved Permanently\nLocation: ";
				strcat(movedError, file);
				strcat(movedError, "/");
				errorOccured++;
				errorMessage = strdup(movedError);

				#ifdef DEBUG_COMPILE_MODE
				printf("redirected from: %s\nTo: %s\n", file, movedError);
				#endif
			}else{
				char* folderStartCharacter = strchr(file, '/')+1;
				char* folderEndCharacter = strchr(folderStartCharacter, '/');
				int dynamicsFolderSize = (folderEndCharacter-folderStartCharacter)+1;
				char* dynamicsFolder = malloc(dynamicsFolderSize);
				dynamicsFolder[dynamicsFolderSize-1] = 0;
				memset(dynamicsFolder, 0, dynamicsFolderSize-1);
				strncpy(dynamicsFolder, folderStartCharacter, dynamicsFolderSize-1);

				#ifdef DEBUG_COMPILE_MODE
				printf("folderlen: %d\n", strlen(dynamicsFolder));
				printf("dynamics strlen: %d\n", strlen("dynamics"));
				printf("dynamicsFolder: %s\n", dynamicsFolder);
				#endif

				if(strncmp(dynamicsFolder, "dynamics",strlen("dynamics")) == 0){ //tests to see if the query is a dynamic query
					int fullString = 0;
					char* fileStartCharacter = folderEndCharacter+1;
					char* fileEndCharacter = NULL;
					int dynamicsFileSize = (fileEndCharacter-fileStartCharacter)+1;
					if(((fileEndCharacter = strchr(fileStartCharacter, '/')-1) == NULL) || (dynamicsFileSize<(strlen(fileStartCharacter)+2))){
						fullString = 1;
						fileEndCharacter = fileStartCharacter + strlen(fileStartCharacter);
					}
					dynamicsFileSize = (fileEndCharacter-fileStartCharacter)+1;
					char* dynamicsFile;
					if(fullString == 0){
						dynamicsFile = malloc(dynamicsFileSize+1);
						dynamicsFile[dynamicsFileSize] = 0;
						memset(dynamicsFile, 0, dynamicsFileSize);
						strncpy(dynamicsFile, fileStartCharacter, dynamicsFileSize);
					} else {
						dynamicsFile = malloc(strlen(fileStartCharacter)+1);
						dynamicsFile[strlen(fileStartCharacter)] = 0;
						strcpy(dynamicsFile, fileStartCharacter);
					}
					#ifdef DEBUG_COMPILE_MODE
					printf("filelen: %d\n", strlen(dynamicsFile));
					printf("filename: %s\n", dynamicsFile);
					#endif
					if(strncmp(dynamicsFile,"debugmysql",strlen("debugmysql")) == 0){ //quick debug mysql info screen
						FILE* staticHeaders = fopen("../data/dynamics/debugmysql.headers", "r");
						//get the test file of headers
						fseek(staticHeaders, 0, SEEK_END);
						long headerFileSize = ftell(staticHeaders);
						fseek(staticHeaders, 0, SEEK_SET);
						char* outputHeader = malloc(headerFileSize+1);
						fread(outputHeader, headerFileSize, 1, staticHeaders);
						fclose(staticHeaders);
						outputHeader[headerFileSize] = 0;



						char* debugOutput = query_MYSQLDebugOutput();
						finalLength = headerFileSize + strlen(debugOutput)+strlen(message)+2;
						finalMessage = malloc(finalLength+1);
						finalMessage[finalLength] = 0;
						memset(finalMessage, 0, finalLength);
						strcpy(finalMessage, message);
						strcat(finalMessage, outputHeader);
						strcat(finalMessage, "\n\n");
						strcat(finalMessage, debugOutput);

						free(outputHeader);
					} else if (strncmp(dynamicsFile, "getmaps",strlen("getmaps")) == 0){ // get the maps from the database and send in JSON format
						char* mapsResponceHeaderFile = openAndBufferFile("../data/dynamics/getMaps.headers");

						char* mapPaths = query_MapPaths();
						finalLength = strlen(mapsResponceHeaderFile) + strlen(mapPaths) + strlen(message)+2;
						finalMessage = malloc(finalLength+1);
						finalMessage[finalLength] = 0;
						memset(finalMessage, 0, finalLength);
						strcpy(finalMessage, message);
						strcat(finalMessage, mapsResponceHeaderFile);
						strcat(finalMessage, "\n\n");
						strcat(finalMessage, mapPaths);
						free(mapsResponceHeaderFile);
						free(mapPaths);
					} else if (strncmp(dynamicsFile, "getimos", strlen("getimos")) == 0){ //get the collection of IMOs and send in JSON array
						char* imoResponceHeaderFile = openAndBufferFile("../data/dynamics/getimos.headers");

						char* IMOJSON = query_IMONumbers();
						finalLength = strlen(imoResponceHeaderFile) + strlen(IMOJSON) + strlen(message)+2;
						finalMessage = malloc(finalLength+1);
						finalMessage[finalLength] = 0;
						memset(finalMessage, 0, finalLength);
						strcpy(finalMessage, message);
						strcat(finalMessage, imoResponceHeaderFile);
						strcat(finalMessage, "\n\n");
						strcat(finalMessage, IMOJSON);
						free(imoResponceHeaderFile);
						free(IMOJSON);
					} else if (strncmp(dynamicsFile, "setimo", strlen("setimo")) == 0){ //sets the active IMO into the database
						int querySuccessSetIMO = setCurrentIMO((dynamicsFile+strlen("setimo")));
						char* setimoResponceHeaderFile = openAndBufferFile("../data/dynamics/setimo.headers");
						if(querySuccessSetIMO == 1){
							finalLength = strlen(message) + strlen(setimoResponceHeaderFile) + strlen("{\"successful\":\"True\"}") + 2;
							finalMessage = malloc(finalLength+1);
							finalMessage[finalLength] = 0;
							memset(finalMessage, 0, finalLength);
							strcpy(finalMessage, message);
							strcat(finalMessage, setimoResponceHeaderFile);
							strcat(finalMessage, "\n\n");
							strcat(finalMessage, "{\"successful\":\"True\"}");
						} else {
							finalLength = strlen(message) + strlen(setimoResponceHeaderFile) + strlen("{\"successful\":\"False\"}") + 2;
							finalMessage = malloc(finalLength+1);
							finalMessage[finalLength] = 0;
							memset(finalMessage, 0, finalLength);
							strcpy(finalMessage, message);
							strcat(finalMessage, setimoResponceHeaderFile);
							strcat(finalMessage, "\n\n");
							strcat(finalMessage, "{\"successful\":\"False\"}");
						}
						free(setimoResponceHeaderFile);
					} else if (strncmp(dynamicsFile, "getposition", strlen("getposition")) == 0){ //gets the most recent position report for the set IMO
						char* positionResponce = query_PositionReport();
						char* positionResponceHeader = openAndBufferFile("../data/dynamics/getposition.headers");
						finalLength = strlen(message) + strlen(positionResponceHeader) + strlen(positionResponce) + 2;
						finalMessage = malloc(finalLength+1);
						finalMessage[finalLength] = 0;
						memset(finalMessage, 0, finalLength);
						strcpy(finalMessage, message);
						strcat(finalMessage, positionResponceHeader);
						strcat(finalMessage, "\n\n");
						strcat(finalMessage, positionResponce);
						free(positionResponceHeader);
						free(positionResponce);
					} else {
						errorOccured++;
						errorMessage = "File does not exist";
						#ifdef DEBUG_COMPILE_MODE
						printf("file does not exist\n");
						#endif
					}
					free(dynamicsFile);
				} else {
					errorOccured++;
					errorMessage = "File does not exist";
					#ifdef DEBUG_COMPILE_MODE
					printf("file does not exist\n");
					#endif
				}
				free(dynamicsFolder);
			}
		}
		#ifdef DEBUG_COMPILE_MODE
		printf("file: %s\n", file);
		#endif
		free(path);
		free(headerPath);
		free(file);
	}

	if (errorOccured == 0)
	{ //no error
		send(client_socket_fd, finalMessage, finalLength, 0);
		close(client_socket_fd);
		memset(finalMessage, 0, strlen(finalMessage));
		free(finalMessage);
	} else { //error occured (aka errorOccured is not 0)
		send(client_socket_fd, errorMessage, strlen(errorMessage), 0);
		close(client_socket_fd);
	}

	// remove current thread (aka set address to 0) from list so that slot can be reused
	*activeThread = 0;
	pthread_exit(NULL);
}