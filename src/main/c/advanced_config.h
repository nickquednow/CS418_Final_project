#ifndef ADVANCED_CONFIG_H
#define ADVANCED_CONFIG_H

//the maximum number of headers that should be allowed from a request, if more than this number, the request will error out
#define MAX_HEADERS 30
//the number of http codes that it could respond to
#define NUM_HTTP_CODES 9
//an array of http codes that can be used, they should be quoted as strings and separated by commas
#define HTTP_CODES_ARRAY {\
		"GET",\
		"HEAD",\
		"POST",\
		"PUT",\
		"DELETE",\
		"CONNECT",\
		"OPTIONS",\
		"TRACE",\
		"PATCH"}

#endif //ADVANCED_CONFIG_H