import mysql.connector
from mysql.connector import errorcode

import toml
import json
import time

tomlFile = toml.load("config.toml")

mysql_descelated_logins = tomlFile["mysql_new_user"]
descelated_username = mysql_descelated_logins["username"]
descelated_password = mysql_descelated_logins["password"]
descelated_host = mysql_descelated_logins["host"]
descelated_port = mysql_descelated_logins["port"]

fileLocations = tomlFile["file_locations"]

databaseName = "QUEDNOW_AIS_WEBPAGE"

cnx = mysql.connector.connect(user=descelated_username, password=descelated_password, host=descelated_host, port=descelated_port)

cursor = cnx.cursor()

query = """
USE {};
CREATE TABLE MAP_TILES ( url VARCHAR(150) NOT NULL, x1 DECIMAL(6,3) NOT NULL, y1 DECIMAL(6,3) NOT NULL, width DECIMAL(6,3) NOT NULL, height DECIMAL(6,3) NOT NULL, PRIMARY KEY (url));
""".format(databaseName)

query += """
CREATE TABLE {}.AIS_METADATA ( ais_id INT NOT NULL, time TIMESTAMP NOT NULL, imo DECIMAL(8,0), mmsi DECIMAL(9,0), class VARCHAR(20), PRIMARY KEY (ais_id) );
""".format(databaseName)

query += """
CREATE TABLE {}.POSITIONS ( id INT NOT NULL AUTO_INCREMENT, ais_id INT NOT NULL, x DECIMAL(10,7) NOT NULL, y DECIMAL(10,7) NOT NULL, head DECIMAL(10,7) NOT NULL, PRIMARY KEY(id), FOREIGN KEY (ais_id) REFERENCES AIS_METADATA(ais_id) ON DELETE CASCADE);
""".format(databaseName)

# used for script storage
query += """
CREATE TABLE SCRIPTS ( id INT NOT NULL AUTO_INCREMENT, field VARCHAR(100) NOT NULL, value VARCHAR(100) NOT NULL, PRIMARY KEY (id) );
"""

cursor.execute(query)

print("Successfully created tables: MAP_TILES, AIS_METADATA, POSITIONS, SCRIPTS")

cursor.close()
cnx.close()