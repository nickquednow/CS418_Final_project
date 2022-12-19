'use strict';
//current values for the users window
var viewport_specs = {
    width: 0,
    height: 0
};
//current values for the user canvas SVG
var current_positions = {
    x: 0,
    y: 0,
    start_drag_x: 0,
    start_drag_y: 0,
    start_drag_x_window: 0,
    start_drag_y_window: 0,
    scale: 0.5
};
//current elements for the edit menu and max/min length attributes
var edit_menu_elements = {
    title: null,
    imo: null,
    current_group: null,
    title_character_bounds: {
        maximum: 14,
        minimum: 0
    },
    short_description_character_bounds: {
        maximum: 19,
        minimum: 0
    },
    currentIMO:0
}


var user_workspace = null; //user canvas SVG element

var position_marker_element = null;

const copySign = (x, y) => Math.sign(x) === Math.sign(y) ? x : -x;

window.resize = event => {
    resize_window();
};

/**
 * loads default configs onto the screen
 * 
 * what:
 * - current window area
 * - event listeners
 * - global variables of elements
 */
function onLoad() {
    initialize_global_variables();
    set_max_string_lengths();
    resize_window(current_positions.x, current_positions.y, current_positions.scale);
    add_event_listeners();
    window.addEventListener("resize", function(){
        resize_window(current_positions.x, current_positions.y, current_positions.scale);
    });
    startWorker();
    getMapTiles();
    getIMOArray();
    putIMOOptions();
}

/**
 * sets globally accessable variables
 */
function initialize_global_variables(){
    user_workspace = document.getElementById("user_window");

    position_marker_element = user_workspace.querySelector("g#positionMarker");

    edit_menu_elements.main = document.querySelector("div#editMenu");
    edit_menu_elements.title = editMenu.querySelector("#innerContent #title");
    edit_menu_elements.imo = edit_menu_elements.main.querySelector("#innerContent #imo");
}

/**
 * makes sure that the maximum string length is shown on the edit menu
 */
function set_max_string_lengths(){
    edit_menu_elements.main.querySelector("#titleMaxLength").innerText = edit_menu_elements.title_character_bounds.maximum;
}

/**
 * adds event listeners to the user canvas and elements on canvas
 */
function add_event_listeners() {
    window.addEventListener("mousedown", start_dragging_viewport);
    window.addEventListener("wheel", scale_window);
    document.querySelector("svg#user_window").addEventListener("contextmenu", show_edit_menu);
}

/**
 * removes event listeners on the user canvas and elements on canvas to allow for edit menu
 */
function removeEventListeners() {
    window.removeEventListener("mousedown", start_dragging_viewport);
    window.removeEventListener("wheel", scale_window);
    document.querySelector("svg#user_window").removeEventListener("contextmenu", show_edit_menu);
}

/**
 * 
 * done to start dragging canvas so we are not always calculating the mouse delta
 * - preformance savings and allows user to start dragging from different position
 * - grabs starting positions for delta (or change) calculations
 * 
 * @param {event} e event class of function (only uses clientY and clientX for mouse position and grabs the button pressed)
 */
function start_dragging_viewport(e) {
    //if you right click dont move viewport instead show edit menu
    if (e.button == 2) {
        return;
    }
    //stores current position in the canvas and the user window
    current_positions.start_drag_x_window = e.clientX;
    current_positions.start_drag_y_window = e.clientY;
    current_positions.start_drag_x = current_positions.x;
    current_positions.start_drag_y = current_positions.y;
    //start the canvas movements
    window.addEventListener("mousemove", move_window);
    //stops the canvas movements
    window.addEventListener("mouseup", drop_window);
}

/**
 * calculates the change in the mouse position and moves canvas accordingly
 * - use deltas (or change) and the current canvas width to calculate the new viewport
 * - makes sure that the user does not go into the negatives because it is an invalid region
 * 
 * @param {Event} e 
 */
function move_window(e) {
    //calculate deltas and makes sure that the top left corner does not go negative (invalid region)
    current_positions.x = Math.min(Math.max((current_positions.start_drag_x - ((((e.clientX - current_positions.start_drag_x_window) / (viewport_specs.width*current_positions.scale)) * 2)/*(2/current_positions.scale)*/)), 7.0),12.75);
    current_positions.y = Math.min(Math.max((current_positions.start_drag_y - ((((e.clientY - current_positions.start_drag_y_window) / (viewport_specs.height*current_positions.scale)) * 2)/*(2/current_positions.scale)*/)), 54.75),57.75);
    //send data to resize the window
    resize_window(current_positions.x, current_positions.y, current_positions.scale);
}

/**
 * stops calculating deltas and removes added event listeners from start_dragging_viewport()
 */
function drop_window() {
    window.removeEventListener("mouseup", drop_window);
    window.removeEventListener("mousemove", move_window);
    current_positions.start_drag_x_window = 0;
    current_positions.start_drag_y_window = 0;
    current_positions.start_drag_x = 0;
    current_positions.start_drag_y = 0;
}

/**
 * calculates the new scale of window *ONLY WORKS WITH MOUSE WHEELS*
 * 
 * @param {event} e wheel event from the event listener (only uses deltaY)
 */
function scale_window(e) {
    var y = e.deltaY; //distance of the mouse scroll (will be useful for laptop users in the future)
    var temp_scale;
    //makes it an exponential curve the more you get closer to 0 and a linear curve after scale of 1
    if (current_positions.scale <= 1) {
        temp_scale = + Math.pow(2, ((Math.log(current_positions.scale) / Math.log(2)) + (-1 * copySign(1, y)))); //for if the scale is greater than 100%
    } else {
        temp_scale = current_positions.scale + (-1 * copySign(1, y));
    }
    current_positions.scale = temp_scale;
    resize_window(current_positions.x, current_positions.y, current_positions.scale); //sets position with new scale
}

/**
 * displays edit menu and removes listeners for scaling and moving the canvas
 * 
 * @param {Event} e mouse event that prevents the right click menu from showing up
 */
function show_edit_menu(e) {
    e.preventDefault();

    var targetElement = e.target || e.srcElement

    //gets tag name, and if it is not the parent group, put the parent group in the "current_group" variable
    switch(targetElement.tagName.toUpperCase()){
        case "TEXT":
        case "RECT":
            edit_menu_elements.current_group = targetElement.parentElement; //using srcElement for compatibility with older browsers (down to IE8)
            break;
        default:
            edit_menu_elements.current_group = targetElement;
            break;
    }

    //prefill the content with the current title and short description
    edit_menu_elements.imo.value = edit_menu_elements.currentIMO;

    //ensuret the character count is accurate when the menu loads
    add_counters();

    //allows the edit menu to be centered
    edit_menu_elements.main.style.display = "flex";
    //removes translation and scaling event listeners
    removeEventListeners();
    //makes sure that if the user clicks away off the edit menu it hides and saves the changes
    edit_menu_elements.main.addEventListener("click", hide_edit_menu);
    //detects if the user has clicked the edit menu to make sure that it does not hide when real events are happening
    edit_menu_elements.main.children[0].addEventListener("click", detect_edit_options_pressed);
    //alows for the counters to go up when key is pressed
    document.addEventListener("keyup", add_counters);
}

function add_counters(){
}

/**
 * this is to make sure that if the user clicks the edit menu it does not close
 * 
 * @param {Event} e mouse event to prevent the bubbling behavior
 */
function detect_edit_options_pressed(e) {
    e.stopPropagation();
}

/**
 * resets all listeners to their original state before editing
 */
function hide_edit_menu() {
    edit_menu_elements.main.children[0].removeEventListener("click", detect_edit_options_pressed);
    edit_menu_elements.main.removeEventListener("click", hide_edit_menu);
    document.removeEventListener("keyup", add_counters);
    add_event_listeners();
    editMenu.style.display = "none";
}

/**
 * handles the changing of the title and short description of the SVG element
 */
function submit_edit_menu(){
    currentIMO = edit_menu_elements.imo.value;
    if(currentIMO == "none"){
        currentIMO = 0;
    }
    currentIMO = currentIMO.toString().padStart(7,"0");
    updateFollowedIMO(currentIMO);
    hide_edit_menu();
}

/**
 * 
 * @param start_x the x-value of the top left corner
 * @param start_y the y-value of the top left corner
 */
function resize_window(start_x, start_y, scale = 1) {
    var w = window.innerWidth;
    var h = window.innerHeight;
    viewport_specs.width = w;
    viewport_specs.height = h;
    var r = w / h
    user_workspace.setAttribute("viewBox", start_x + " " + start_y + " " + (5 * (1 / scale)) + " " + ((5 / r) * (1 / scale))); //5 to allow for default scaling
}

function putIMOOptions(){
    edit_menu_elements.imo.innerHTML = '<option value="none">none</option>';
    var finalString = ""
    var i=0;
    var firstOption = imoArray[0];
    if(imoArray.length == 0){
        getIMOArray();
    }for(i=0;i<imoArray.length;i++){
        finalString += '<option value="' + imoArray[i] + '">' + imoArray[i] + '</option>';
    }
    edit_menu_elements.imo.innerHTML += finalString;
    edit_menu_elements.imo.value = firstOption;
    currentIMO;
}

function setPosition(x,y,head){
    // position_marker_element.children[0].transform.baseVal.getItem(0).setTranslate(x,y);
    // position_marker_element.children[0].transform.baseVal.getItem(0).setRotate(head, 0, 0);
    // position_marker_element.children[0].transform.baseVal.getItem(0).setScale(0.25, 0.25);
    position_marker_element.children[0].setAttribute("transform", "scale(0.25, 0.25) translate("+x+","+y+") rotate("+head+")");
    // position_marker_element.innerHTML = "<polygon fill=\"red\" points=\"0,-0.5 0.5,0.5 0,0.3 -0.5,0.5\" transform=\"translate(" + x + "," + y + ") scale(0.25,0.25) rotate(" + head + ")\"></polygon>";
    // console.log(position_marker_element.outerHTML);
}