import mysql.connector
from mysql.connector import errorcode

import toml

tomlFile = toml.load("config.toml")
mysql_logins = tomlFile["mysql_admin_login"]
username = mysql_logins["username"]
password = mysql_logins["password"]
host = mysql_logins["host"]
port = mysql_logins["port"]

mysql_descelated_logins = tomlFile["mysql_new_user"]
descelated_username = mysql_descelated_logins["username"]
descelated_password = mysql_descelated_logins["password"]
descelated_host = mysql_descelated_logins["host"]
descelated_port = mysql_descelated_logins["port"]
descelated_created = mysql_descelated_logins["exists"]

if username == "":
    print("username not supplied. read README")
    exit(1)

cnx = mysql.connector.connect(user=username, password=password, host=host, port=port)

if not descelated_created:
    cursor = cnx.cursor()
    query = ""
    if password == "":
        query = "CREATE USER '" + descelated_username + "'@'" + descelated_host + "';"
    else: 
        query = "CREATE USER '" + descelated_username + "'@'" + descelated_host + "' IDENTIFIED BY '" + descelated_password + "';"

    cursor.execute(query)

    query = "GRANT ALL TO USER '" + descelated_username + "'@'" + descelated_host + "' ON QUEDNOW_AIS_WEBPAGE.*;"

    cursor.execute(query)
    cursor.close()
cnx.close()