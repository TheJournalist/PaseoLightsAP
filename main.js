// Parameters
var sitePath = "www";
var port = 80;

"use strict" ;
// Libraries
var m = require('mraa');
var fs = require('fs');
var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var five = require('johnny-five');
var Edison = require('edison-io');
var sys = require('sys')
var exec = require('child_process').exec;
var child;
var cfg = require("./utl/cfg-app-platform.js")() ; 

function sendCMD(data)
{
  cfg.io.writeStr(data.toString());
}

// Create a new Johnny-Five board object
var board = new five.Board({
    io: new Edison()
});

// Initialization callback that is called when Johnny-Five is done initializing
board.on('ready', function() {
    var exec = require('child_process').exec;
    var cmd = 'configure_edison --enableOneTimeSetup';
    exec(cmd, function(error, stdout, stderr) {});
});

// Request logging
app.use(function(req, res, next) {
    if (req.url === '/') {
        console.log("Serving page");
    }
    next();
});

// Create a handler for when a client connects via socket.io
io.on('connection', function(socket) {
    var clientIP = socket.client.conn.remoteAddress;
  
    socket.on('command', function(command) {
        console.log(clientIP + ": COMANDO " + command);
        sendCMD(command);
    });
    
});

// confirm that we have a version of libmraa and Node.js that works
// prints some interesting platform details to console

cfg.identify() ;                // prints some interesting platform details to console

if( !cfg.test() ) {
    console.log("UART TEST ERROR!!! HU3");
}

if( !cfg.init() ) {
    console.log("UART INIT ERROR!!! HU3");
}
cfg.io = new cfg.mraa.Uart(cfg.ioPin);         // construct our I/O object
cfg.ioPath = cfg.io.getDevicePath();           // get path to UART device
if( typeof cfg.ioPin === "number" && Number.isInteger(cfg.ioPin) ) {
    console.log("UART mraa #: " + cfg.ioPin);
    console.log("UART" + cfg.ioPin + " device path: " + cfg.ioPath);
} else {
    console.log("UART has no mraa #, using: " + cfg.ioPin);
    console.log("UART device path: " + cfg.ioPath);
}
cfg.io.setBaudRate(115200);
cfg.io.setMode(8, cfg.mraa.UART_PARITY_NONE, 1);
cfg.io.setFlowcontrol(false, false);
cfg.io.setTimeout(0, 0, 0);  

// Start server
console.log(sitePath);
console.log("Starting server in: " + __dirname + '/' + sitePath);
app.use(express.static(__dirname + '/' + sitePath));
http.listen(port, function() {
    console.log("Server running at: http://localhost:" + port);
});
