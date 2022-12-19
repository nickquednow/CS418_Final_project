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

mapTilesFileLocation = fileLocations["mapViewJSON"]
mapTilesFile = open(mapTilesFileLocation, 'r')
mapTilesFileLines = mapTilesFile.readlines()

values = ""

count = 0
# Strips the newline character
for line in mapTilesFileLines:
    count += 1
    lineParsed = json.loads(line.strip())
    if lineParsed["scale"] == 3:
        filename = lineParsed["filename"]
        url = "https://cs418.assets.cloudevelop.dev/maps/" + filename
        north = lineParsed["north"]
        south = lineParsed["south"]
        west = lineParsed["west"]
        east = lineParsed["east"]
        values += "( '{}' , {} , {} , {} , {} ), ".format( url, west, north, (east - west), (north - south))

valuesInserted = values[:-2]

query = """
INSERT INTO {}.MAP_TILES ( url, x1, y1, width, height) VALUES {};
""".format(databaseName, valuesInserted)
# print(query)

cursor.execute(query)
cnx.commit()
print(cursor.rowcount, "Records inserted successfully into MAP_TILES table")

values = ""

def constructPositionInsertions(PositionJSON):
    global values
    array = json.loads(PositionJSON)
    Id = array["AISMessage_Id"]
    navStats = array["NavigationalStatus"]
    Long = array["Longitude"]
    Lat = array["Latitude"]
    RoT = array["RoT"]
    SoG = array["SoG"]
    CoG = array["CoG"]
    Heading = array["Heading"]
    LastStaticData_Id = array["LastStaticData_Id"]
    MapView1_Id = array["MapView1_Id"]
    MapView2_Id = array["MapView2_Id"]
    MapView3_Id = array["MapView3_Id"]
    if Id == "" or Long == "" or Lat == "" or Heading == "":
        return
    if navStats == "Under way using engine":
        # ais_id, x, y, head
        values += "( {}, {}, {}, {} ),".format(Id, Long, Lat, Heading)

def constructAISMetadataInsertions(metadataJSON):
    global values
    array = json.loads(metadataJSON)
    id = array["Id"]
    timestamp = array["Timestamp"]
    mmsi = array["MMSI"]
    class_val = array["Class"]
    vesselIMO = array["Vessel_IMO"]
    if mmsi == "" or timestamp == "" or id == "":
        return
    if class_val == "":
        class_val = "NULL"
    else:
        class_val = "\'{}\'".format(class_val)
    if vesselIMO == "":
        vesselIMO = "NULL"
    else:
        vesselIMO = "{}".format(vesselIMO)
    # ais_id, time, mmsi, imo, class
    values += "( {}, '{}', {}, {}, {} ),".format(id, timestamp, mmsi, vesselIMO, class_val)

def csv_to_json(headers, data, seperator=","):
    tags = headers.split(seperator)
    values = data.split(seperator)
    finalOut = "{"
    i=0
    for t in tags:
        if values[i].replace("\n", "") != "\\N":
            finalOut += "\"{}\":\"{}\",".format(t.replace("\n", ""), values[i].replace("\n", ""))
        else:
            finalOut += "\"{}\":\"{}\",".format(t.replace("\n", ""), "")
        i+=1
    finalOut = finalOut[:-1]
    finalOut+="}\n"
    return finalOut

AIS_Messages = open(fileLocations["AISMetaData"], "r")
AIS_Position_Messages = open(fileLocations["AISPositionReports"], "r")

AIS_Messages_Lines = AIS_Messages.readlines()
AIS_Position_Messages_Lines = AIS_Position_Messages.readlines()

AIS_Messages_Line_one = AIS_Messages_Lines[0]
AIS_Position_Messages_Line_one = AIS_Position_Messages_Lines[0]
i=0


values = ""

#AIS Metadata
i=0
for line in AIS_Messages_Lines:
    if i==0:
        i+=1
        continue
    if (i-1)%50000 == 0 and i>1: #prevents hitting the data limit adjust if needed
        query = """
        INSERT INTO {}.AIS_METADATA (ais_id, time, mmsi, imo, class) VALUES {};
        """.format(databaseName, values[:-1])
        # print(query)
        cursor.execute(query)
        cursor.reset(True)
        print("Records inserted successfully into AIS_METADATA table. {} records sorted".format(i))
        cnx.commit()
        query = ""
        values = ""
    constructAISMetadataInsertions(csv_to_json(AIS_Messages_Line_one, line, ";"))
    i+=1


if values != "":
    # print(values[:-1])
    query = """
    INSERT INTO {}.AIS_METADATA (ais_id, time, mmsi, imo, class) VALUES {};
    """.format(databaseName, values[:-1])
    # print(query)
    cursor.execute(query)
    cursor.reset(True)
    print("Records inserted successfully into AIS_METADATA table. {} records sorted".format(i))
    cnx.commit()

values=""
i=0

for line in AIS_Position_Messages_Lines:
    if i==0:
        i+=1
        continue
    if i%50000 == 0:#prevents hitting the data limit adjust if needed
        query = """
        INSERT INTO {}.POSITIONS (ais_id, x, y, head) VALUES {};
        """.format(databaseName, values[:-1])
        # print(query)
        cursor.execute(query)
        cursor.reset(True)
        print("Records inserted successfully into POSITION table. {} records sorted".format(i))
        cnx.commit()
        query = ""
        values = ""
    constructPositionInsertions(csv_to_json(AIS_Position_Messages_Line_one, line, ";"))
    i+=1


if values != "":
    query = """
    INSERT INTO {}.POSITIONS (ais_id, x, y, head) VALUES {}
    """.format(databaseName, values[:-1])

    cursor.execute(query)
    cursor.reset(True)
    print("Records inserted successfully into POSITION table. {} records sorted".format(i))
    cnx.commit()

query = """
INSERT INTO {}.SCRIPTS (field, value) VALUES ('cleanTimeStamp','2020-11-18 00:00:00'),('currentIMO','0');
""".format(databaseName)


cursor.close()
cnx.close()

