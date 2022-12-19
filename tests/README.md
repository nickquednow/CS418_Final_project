# Test Scripts
There is only a webserver test in here because that is all that I have tried to test (everything else is through the web interface)

## Setup
Please go into the test.sh script and edit the variables at the top of the script
- there is one for the python executable
- one is for the data directory (good to have the absolute path, but can be a relative path. Expect weird behavior if using a relative path)

## test_webserver.py
This just tests to see if the webserver is up and running correctly