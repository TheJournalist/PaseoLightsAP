// Parameters
var sitePath = "www";
var port = 80;
var highscoresFile = "www/highscores.json"

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

// Read high scores
var scoreContent = fs.readFileSync(__dirname + "/" + highscoresFile);
scoreContent = JSON.parse(scoreContent);
console.log(scoreContent);

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

//app.get('/light.html', function (req, res) {
//  res.sendFile(__dirname + '/www/light.html');
//});

//app.get('/game.html', function (req, res) {
//  res.sendFile(__dirname + '/www/game.html');
//});

//app.get('/navi.html', function (req, res) {
//  res.sendFile(__dirname + '/www/navi.html');
//});

// Date
function getDateTime() {
    var date = new Date();
    var hour = date.getHours();
    hour = (hour < 10 ? "0" : "") + hour;
    var min  = date.getMinutes();
    min = (min < 10 ? "0" : "") + min;
    var sec  = date.getSeconds();
    sec = (sec < 10 ? "0" : "") + sec;
    var year = date.getFullYear();
    var month = date.getMonth() + 1;
    month = (month < 10 ? "0" : "") + month;
    var day  = date.getDate();
    day = (day < 10 ? "0" : "") + day;
    return year + ":" + month + ":" + day + ":" + hour + ":" + min + ":" + sec;
}

// Create a handler for when a client connects via socket.io
io.on('connection', function(socket) {
    var clientIP = socket.client.conn.remoteAddress;
  
    socket.on('command', function(command) {
        console.log(getDateTime() + " - " + clientIP + " - cmd: " + command);
        cfg.io.writeStr(String.fromCharCode(command));
        cfg.io.writeStr(String.fromCharCode(command));
        cfg.io.writeStr(String.fromCharCode(command));
        cfg.io.writeStr(String.fromCharCode(command));
    });
    /*
    // If we get a score, compare it to other scores
    socket.on('score', function(score) {
        console.log(clientIP + ": " + score);
        if (score > scoreContent.highscores[scoreContent.highscores.length - 1].score) {
            console.log("High score! Requesting initials");
            socket.emit('initialsRequest', '');
        }
    });
    
    // If we get initials back, store them with the high scores
    socket.on('initials', function(msg) {
        console.log(msg + ": " + lastScore);
        
        // Check for top score
        if (lastScore > scoreContent.highscores[0].score) {
            scoreContent.highscores.splice(0, 0, {
                        "initials": msg, 
                        "score": lastScore
            });
            scoreContent.highscores.splice(-1, 1);
            
        // Check for other placement in high scores
        } else {
            for (var i = scoreContent.highscores.length - 1; i >= 0; i--) {
                if ((lastScore <= scoreContent.highscores[i].score) || 
                    (i === 0)) {
                    if (i === scoreContent.highscores.length - 1) {
                        scoreContent.highscores.push({
                            "initials": msg, 
                            "score": lastScore
                        });
                    } else {
                        scoreContent.highscores.splice(i + 1, 0, {
                            "initials": msg, 
                            "score": lastScore
                        });
                    }
                    console.log(scoreContent.highscores);
                    scoreContent.highscores.splice(-1, 1);
                    break;
                }
            }
        }
        console.log(scoreContent.highscores);
        */
        // Write high scores to file
        fs.writeFile(__dirname + "/" + highscoresFile, 
                     JSON.stringify(scoreContent, null, 4), function(err) {
            if (err) {
                console.log("Cound not save high scores: " + err);
            } else {
                console.log("High scores saved");
            }
        });
    })
});

// confirm that we have a version of libmraa and Node.js that works
// prints some interesting platform details to console

cfg.identify() ;                // prints some interesting platform details to console

if( !cfg.test() ) {
    console.log("UART TEST ERROR!!!");
}

if( !cfg.init() ) {
    console.log("UART INIT ERROR!!!");
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
cfg.io.setBaudRate(9600);
cfg.io.setMode(8, cfg.mraa.UART_PARITY_NONE, 1);
cfg.io.setFlowcontrol(false, false);
cfg.io.setTimeout(0, 0, 0);  
/*
var lastScore;

setInterval(function(){
    var len = cfg.io.dataAvailable(0);
    if(len)
    {
        var data = cfg.io.readStr(len);
        lastScore = data;
        socket.emit('initialsRequest', '');
        console.log(" Game score: " + data);
    }
}, 100);
*/
// Start server
console.log(sitePath);
console.log("Starting server in: " + __dirname + '/' + sitePath);
app.use(express.static(__dirname + '/' + sitePath));
http.listen(port, function() {
    console.log("Server running at: http://localhost:" + port);
});
