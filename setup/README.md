# Setup scripts
This will take a while, so please be patient
## mysql_admin_login
This user must exist already

`username` - MYSQL admin username
`password` - MYSQL admin password
`host` - MYSQL admin host
- default installation is localhost
`port` - MYSQL admin port
- default installation is 3306

## mysql_new_user
This is the user that will be created (is the option is specified to do so)

`username` - MYSQL new user username
`password` - MYSQL new user password
`host` - MYSQL new user host
- default installation is localhost
`port` - MYSQL new user port
- default installation is 3306
`exists` - set to true if the user already exists
- This will skip user creation if set to true (put the user credentials above if that is the case)
- the user must have `INSERT, SELECT, DROP, and DELETE` privelages (will create user will `ALL` privilages for simplicity on this database only).

## Errors
If you have an error saying somethign about hitting a data rate limit, go into `insert.py` and on line 133 and line 166, make the number `50000` smaller until you lo longer hit rate limits