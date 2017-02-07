// Parameters
var sitePath = "www";
var port = 80;

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

var D0 = new m.Gpio(45); 
D0.dir(m.DIR_OUT);
D0.write(0);

var D1 = new m.Gpio(32); 
D1.dir(m.DIR_OUT);
D1.write(0);

var D2 = new m.Gpio(46); 
D2.dir(m.DIR_OUT);
D2.write(0);

var D3 = new m.Gpio(33); 
D3.dir(m.DIR_OUT);
D3.write(0);

var D4 = new m.Gpio(47); 
D4.dir(m.DIR_OUT);
D4.write(0);

var D5 = new m.Gpio(48); 
D5.dir(m.DIR_OUT);
D5.write(0);

var D6 = new m.Gpio(36); 
D6.dir(m.DIR_OUT);
D6.write(0);

var D7 = new m.Gpio(14); 
D7.dir(m.DIR_OUT);
D7.write(0);

var CLK = new m.Gpio(31); 
CLK.dir(m.DIR_OUT);
CLK.write(0);

function sendCMD(data)
{
  D0.write(data & 0x01);
  D1.write((data & 0x02) >> 1);
  D2.write((data & 0x04) >> 2);
  D3.write((data & 0x08) >> 3);
  D4.write((data & 0x16) >> 4);
  D5.write((data & 0x32) >> 5);
  D6.write((data & 0x64) >> 6);
  D7.write((data & 0x128) >> 7);
  
  CLK.write(1);
  setTimeout(fall, 100);
}

function fall()
{
  CLK.write(0);
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

// Start server
console.log(sitePath);
console.log("Starting server in: " + __dirname + '/' + sitePath);
app.use(express.static(__dirname + '/' + sitePath));
http.listen(port, function() {
    console.log("Server running at: http://localhost:" + port);
});
//sendCMD(0x04);
