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

var fileName = "make-schedule/worker.js"


function getAllIndexes(arr, val) {
    var indexes = [], i;
    for (i = 0; i < arr.length; i++)
        if (arr[i] === val)
            indexes.push(i);
    return indexes;
}

function sendToLog(data) {
    postMessage(workerCallbacks.delimiter + workerCallbacks.logData + workerCallbacks.delimiter + data)
}

function errorMessage(data){
    sendToLog("ERROR: " + data);
}

function returnNewTile(data){
    splitData = data.split("\n");
    for(i in splitData){
        if(splitData[i] == "")
            continue;
        postMessage(workerCallbacks.delimiter + workerCallbacks.newMapTile + workerCallbacks.delimiter + splitData[i]);
    }
}

function returnIMONumbers(data){
    postMessage(workerCallbacks.delimiter + workerCallbacks.allIMOs + workerCallbacks.delimiter + data);
}

function returnSuccessfulIMO(data){
    newJSON = JSON.parse(data);
    success = (newJSON.successful == "True" ? 1 : 0);
    console.log(success);
    console.log(newJSON.successful);
    postMessage(workerCallbacks.delimiter + workerCallbacks.returnSuccessUpdate + workerCallbacks.delimiter + success);
}

function returnPosition(data){
    postMessage(workerCallbacks.delimiter + workerCallbacks.setPosition + workerCallbacks.delimiter + data);
}

function getMapTiles(json) {
    newJSON = JSON.parse(json);
    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", function () {
        returnNewTile(this.responseText);
    });
    oReq.open("GET", "/dynamics/getmaps/");
    oReq.setRequestHeader("content-type", "text/json");
    oReq.send()
}

function getIMOArray(json){
    newJSON = JSON.parse(json);
    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", function () {
        returnIMONumbers(this.responseText);
    });
    oReq.open("GET", "/dynamics/getimos/");
    oReq.setRequestHeader("content-type", "text/json");
    oReq.send()
}

function pushNewIMO(json){
    newJSON = JSON.parse(json);
    var newIMO = newJSON.imo;
    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", function () {
        returnSuccessfulIMO(this.responseText);
    });
    oReq.open("GET", "/dynamics/setimo" + newIMO + "/");
    oReq.setRequestHeader("content-type", "text/json");
    oReq.send()
}

function getNewPosition(json){
    newJSON = JSON.parse(json);
    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", function () {
        returnPosition(this.responseText);
    });
    oReq.open("GET", "/dynamics/getposition/");
    oReq.setRequestHeader("content-type", "text/json");
    oReq.send()
}

var funct_arr = [];

funct_arr[workerCalls.getMapTiles] = getMapTiles;
funct_arr[workerCalls.getIMOs] = getIMOArray;
funct_arr[workerCalls.updateIMO] = pushNewIMO;
funct_arr[workerCalls.getPosition] = getNewPosition;
onmessage = function (e) {
    // const functionText = getAllIndexes(e.data);
    // const calledFunction = funct_arr[e.data.slice(1, functionText[0])];
    // if (!calledFunction) {
    //     errorMessage("The function that was called through the message was not valid. File: " + fileName);
    // }
    // window[funct_arr[e.data.slice(1, functionText[0])]](e.data.slice(functionText[0] + 1));
    funct_arr[e.data.slice(1, e.data.indexOf("~", 1))](e.data.slice(e.data.indexOf("~", 1) + 1));
};