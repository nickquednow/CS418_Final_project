const workerCalls = {
    getMapTiles: "getMapTiles",

    getIMOs : "getShipIMOs",

    updateIMO: "updateIMO",

    getPosition: "getPosition",

    /**
     * The standard delimeter for communication to worker (before command and after command)
     */
    delimiter: "~"
};

const workerCallbacks = {
    /**
     * - send a message to the log
     * - option: of the message to put to the log
     */
    logData: "LogEvent",

    /**
     * -send map tiles to main thread
     */
    newMapTile: "mapTiles",

    /**
     * send IMOs to main thread in JSON format
     */
    allIMOs: "IMOs",

    /**
     * return position of the selected ship
     */
    setPosition: "setPosition",

    /**
     * return success state of IMO update
     */
    returnSuccessUpdate: "successIMO",
    /**
     * The standard delimeter for communication to worker (before command and after command)
     */
    delimiter: "~"
};

var imoArray = [];
var currentIMO;
function getAllIndexes(arr, val) {
    var indexes = [], i;
    for(i = 0; i < arr.length; i++)
        if (arr[i] === val)
            indexes.push(i);
    return indexes;
}

function getData(command, option = ""){
    w.postMessage(workerCalls.delimiter + command + workerCalls.delimiter + option);
}

function getMapTiles(){
    getData(workerCalls.getMapTiles, '{"data":"none"}');
}

function getIMOArray(){
    getData(workerCalls.getIMOs, '{"data":"none"}');
}

function updateFollowedIMO(imo){
    getData(workerCalls.updateIMO, '{"imo":' + imo + '}');
}

function getPositionUpdate(){
    getData(workerCalls.getPosition, '{"data":"none"}');
}

function errorMessage(data){
    console.error(data)
}

function stopWorker() {
    w.terminate();
    w = undefined;
}

function startWorker() {
    if (typeof (Worker) !== "undefined") {
        if (typeof (w) == "undefined") {
            w = new Worker("worker.js");
        }
    } else {
        document.getElementById("result").innerHTML = "Sorry! No Web Worker support.";
    }
}
startWorker();

var funct_arr = [];
funct_arr[workerCallbacks.logData] = LogEvent;
funct_arr[workerCallbacks.newMapTile] = addMapTile;
funct_arr[workerCallbacks.allIMOs] = storeIMOs;
funct_arr[workerCallbacks.returnSuccessUpdate] = verifySuccessfulIMOUpdate;
funct_arr[workerCallbacks.setPosition] = newPosition;
w.onmessage = function (e) {
    if(funct_arr[(e.data.slice(1, e.data.indexOf("~", 1)))]){
        funct_arr[e.data.slice(1, e.data.indexOf("~", 1))](e.data.slice(e.data.indexOf("~", 1) + 1));
    } else {
        console.error("Function not defined fom Worker: " + e.data.slice(1, e.data.indexOf("~", 1)));
    }
};

function LogEvent(data) {
    console.log(data);
}

function addMapTile(data){
    newJSON = JSON.parse(data);
    x = newJSON.x1;
    y = newJSON.y1;
    wid = newJSON.width;
    hei = newJSON.height;
    url = newJSON.url;
    newElement = '<image x="' + ((((x*2)-14)*0.25)+7.0) + '" y="' + y + '" width="' + (wid/2) + '" height="' + hei + '" xlink:href="' + url + '"/>';
    document.querySelector("svg#user_window").innerHTML += newElement;
}

function addBoatMarker(data){
    newJSON = JSON.parse(data);
    x = newJSON.x;
    y = newJSON.y;
    head = newJSON.head;
    newElement = '<polygon points="0,-0.5 0.5,0.5 0,0.3 -0.5,0.5" transform="translate(' + x + ',' + y + ') scale(0.5,0.5) rotation(' + head + ')"/>';
    document.querySelector("svg#user_window").innerHTML += newElement;
}

function storeIMOs(data){
    newJSON = JSON.parse(data);
    for(j in newJSON.imo)
        imoArray.push(newJSON.imo[j])
    putIMOOptions();
}

function verifySuccessfulIMOUpdate(data){
    if(data == "0"){
        alert("failed to update IMO");
        errorMessage("Failed to Update IMO");
    } else {
        alert("successful update for IMO");
    }
}

function newPosition(data){
    console.log(data);
    newJSON = JSON.parse(data);
    console.log(newJSON.x_pos);
    console.log(newJSON.y_pos);
    console.log(newJSON.head);
    setPosition(newJSON.x_pos, newJSON.y_pos, newJSON.head);
}