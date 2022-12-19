import mysql.connector
from mysql.connector import errorcode

import toml

tomlFile = toml.load("config.toml")
mysql_logins = tomlFile["mysql_admin_login"]
username = mysql_logins["username"]
password = mysql_logins["password"]
host = mysql_logins["host"]
port = mysql_logins["port"]

if username == "":
    print("username not supplied. read README")
    exit(1)

cnx = mysql.connector.connect(user=username, password=password, host=host, port=port)

cursor = cnx.cursor()

query = "DROP DATABASE IF EXISTS QUEDNOW_AIS_WEBPAGE;"

cursor.execute(query)

query = "CREATE DATABASE QUEDNOW_AIS_WEBPAGE;"

cursor.execute(query)

cursor.close()
cnx.close()