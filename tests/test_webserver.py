# This is just to see if the index.html file is read correctly
import pycurl
from io import BytesIO
import toml

settings = toml.load("config.toml")
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("toml file loaded\n")

passedColor = "\u001b[32m"
failedColor = "\u001b[31m"
resetColor = "\u001b[0m"

# Creating a buffer as the cURL is not allocating a buffer for the network response
buffer = BytesIO()
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("curl buffer created\n")
c = pycurl.Curl()
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("curl initialized\n")
#initializing the request URL
c.setopt(c.URL, settings["host"]["domain"] + ":" + str(settings["host"]["port"]) + "/example.html")
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("curl webpage URL loaded\n")
#setting options for cURL transfer  
c.setopt(c.WRITEDATA, buffer)
c.setopt(c.HEADER, True)
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("curl webpage options loaded\n")
# perform file transfer
c.perform()
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("curl webpage fetch executed\n")
#Ending the session and freeing the resources
c.close()
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("curl resources closed\n")

body = buffer.getvalue()
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("getting value from buffer\n")

actualFile = open(settings["staticFiles"]["projectRoot"] + "/" + settings["staticFiles"]["dataDirectory"] + "/example.html", "r")
actualHeader = open(settings["staticFiles"]["projectRoot"] + "/" + settings["staticFiles"]["dataDirectory"] + "/example.html.headers", "r")
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("header and data files\n")

finalOutput = "HTTP/1.0 200 OK\n" + actualHeader.read() + "\n\n" + actualFile.read()
if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("concatenate header and data file into proper format\n")

if settings["debug"]["debug"] and settings["debug"]["level"] > 8:
    print("File from server: \n" + body.decode("iso-8859-1") + "\n\n\n\n")
    print("Actual file:\n" + finalOutput + "\n\n\n\n")

if(finalOutput == body.decode("ascii")):
    print(passedColor + "Webserver Test Passed" + resetColor)
else:
    print(failedColor + "Webserver Test Failed" + resetColor)
    failDocument = open("fail.txt", "a")
    failDocument.write("Webserver Test failed\n")

if settings["debug"]["debug"] and settings["debug"]["level"] > 9:
    print("evaluated PASS/FAIL\n")