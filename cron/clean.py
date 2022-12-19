#run every 5 min
import mysql.connector

import toml

tomlFile = toml.load("../setup/config.toml")

mysql_descelated_logins = tomlFile["mysql_new_user"]
descelated_username = mysql_descelated_logins["username"]
descelated_password = mysql_descelated_logins["password"]
descelated_host = mysql_descelated_logins["host"]
descelated_port = mysql_descelated_logins["port"]

fileLocations = tomlFile["file_locations"]

databaseName = "QUEDNOW_AIS_WEBPAGE"

def queryDatabase(query):
    cnx = mysql.connector.connect(user=descelated_username, password=descelated_password, host=descelated_host, port=descelated_port)
    cursor = cnx.cursor()
    cursor.execute(query)
    results = cursor.fetchall()
    cursor.close()
    cnx.close()
    return results

def queryDatabaseNR(query, commit=False):
    cnx = mysql.connector.connect(user=descelated_username, password=descelated_password, host=descelated_host, port=descelated_port)
    cursor = cnx.cursor()
    cursor.execute(query)
    if commit:
        cnx.commit()
    cursor.close()
    cnx.close()

query = """
USE {};
DELETE FROM AIS_METADATA WHERE time < DATE_SUB((SELECT time FROM AIS_METADATA ORDER BY (time) DESC LIMIT 1), INTERVAL 5 MINUTE);
""".format(databaseName)

queryDatabaseNR(query, commit=True)