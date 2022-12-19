#ifndef CONFIG_H
#define CONFIG_H

//port number to listen on
#define PORT 8888
//this is the maximum size that the server will accept when reading a request
#define MAX_BUFFER 4096
//maximum number of connections you want to have connecting to the server at once (each file is essentially a client)
#define MAX_CONNECTIONS 30

#define DATA_DIRECTORY ""

#define MYSQL_DESCELATED_USERNAME ""
#define MYSQL_DESCELATED_PASSWORD ""
#define MYSQL_DESCELATED_HOST ""
#define MYSQL_DESCELATED_PORT 3306
#define MYSQL_DESCELATED_DATABASE "QUEDNOW_AIS_WEBPAGE"

#endif //CONFIG_H