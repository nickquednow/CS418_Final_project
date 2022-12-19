# C configuration
I know it is repetative, but I ran out of time to make an autoconfigure script
keep in mind, you have to run the `./main.o` file in this directory otherwise things may not work

## files

- `config.h` - likely the only file you will need to edit (unless you run into errors)
- `advances_config.h`  - if you run into errors, you can use this

## config.h

`PORT` - What port the server should run on
- any value between 1 and 65535 inclusively
`MAX_BUFFER` - max request size (increase if you are having errors with not enough cache)
- should not have to exceed 8192
`MAX_CONNECTIONS` - max number of requests the server is allowed to process at once
- can be as many as you want, but make sure you have enough memory for a large number
`DATA_DIRECTORY` - the place where the data directory resides (no slash at the end, but including data)
- EX: `../data`
`MYSQL_DESCELATED_USERNAME` "school" - the lower permissions user to query MYSQL (configured in setup TOML)
- can be ADMIN user
`MYSQL_DESCELATED_PASSWORD` "school" - the lower permissions user password to query MYSQL (configured in setup TOML)
- can be ADMIN user
`MYSQL_DESCELATED_HOST` "localhost"
- default installation is `localhost`
`MYSQL_DESCELATED_PORT` - MYSQL port to connect to
- default installation is `3306`
`MYSQL_DESCELATED_DATABASE` "QUEDNOW_AIS_WEBPAGE"

## advanced_config.h
`MAX_HEADERS` - the maximum number of headers the server is allowed to process
- must be at least 1, but can be as big as you would like (it will crash if you do not have enough headers allocated)
`NUM_HTTP_CODES` - the number of HTTP codes that are in the array below
- the standard is 9
`HTTP_CODES_ARRAY` - an array of the available HTTP responce codes
- in case something is missing, feel free to add to this. ***MAKE SURE TO ADD TO `NUM_HTTP_CODES` IF THIS IS CHANGED***