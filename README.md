# CS316 Final Project
## Side Note
I may not have the repository pushed to github, but if things do not work, feel free to contact me so I can push the new code to GitHub.
Contact Email: `nick@cloudevelop.dev`

## Disclosure
I am not responsible for any damages that may be caused on your system if you run this software. By running this you agree that you are proceeding at your own risk and without any warranty.
I have tried to make sure there is no vulnerabilities and is as bug-free as possible, but this does not ensure any damages on ***YOUR*** system will not occur

## Setup
1) proceed to the setup directory
2) look at the README file to see the configurations that you need to make in the TOML file enclosed
3) run the setup.sh file
4) go to `src/main/c/` and read the README file in the event that you need to change any configuration options
5) run make after you have configured the file
6) while inside of the `c` directory, run `valgrind ./main.o`
    a) valgrind is part of the valgrind package. On debian you can use:
    `sudo apt-get install valgrind`
    b) you can run it without `valgrind`, but you may get mixed results (it is serving as a stability layer)
    c) *keep in mind, valgrind will output a bunch of errors, but you can ignore those*
7) the terminal will give you confirmation that the webserver is running and will give you a URL to the running webpage
8) paste the URL into your favorite web browser

## Testing
There are a number (just 1) of testing scripts inside of the root of the workspace folder that may be useful to use.

if you need to know what any of them do, read the README file that is placed there.

if you just want to test everything, run the `./test.sh` file and it will test everything for you. This is useful if there are some errors and you are not sure what is going on.

## Shuting down and cleanup
To stop the server, you just have to use `CTRL+C` to terminate the web server and you can run `make clean` to clean the built files.

## Non-Working (or non-existant) features
- REST interfaces (minus reading data)
- Ship positions on map
- testing scripts

## cloning repository
VIA HTTPS:
`git clone https://github.com/nickquednow/CS418_FinalProject_Clone.git`
VIA SSH:
`git clone git@github.com:nickquednow/CS418_FinalProject_Clone.git`
VIA GitHub CLI:
`gh repo clone nickquednow/CS418_FinalProject_Clone`

# ERRORS!
if there are errors that pop up, feel free to enable verbose logging (I promise it is very verbose, so don't do it lightly)

## Enable verbosity
in `src/main/c/compile_modes.h` comment out `#define PRODUCTION_COMPILE_MODE` and uncomment `#define DEBUG_COMPILE_MODE`

this will output ***EVERYTHING*** that there is to log. this should be a last resort (beyond using gdb, but email me instead if it gets that far)