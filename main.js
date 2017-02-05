// Parameters
var sitePath = "www";
var port = 80;

// Libraries
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
var SerialPort = require("serialport");
var m = require('mraa');
var uart = new m.Uart(0);

var port = new SerialPort("/dev/ttyMFD1", {
  baudRate: 9600
});

port.on('open', function() {
  console.log('serial port opened');
  port.on('data', function(data) {
    console.log('data received:' + data);
  });
});

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
    socket.on('initialsScore', function(msg) {
        console.log(msg.initials + ": " + msg.score);
        
        // Check for top score
        if (msg.score > scoreContent.highscores[0].score) {
            scoreContent.highscores.splice(0, 0, {
                        "initials": msg.initials, 
                        "score": msg.score
            });
            scoreContent.highscores.splice(-1, 1);
            
        // Check for other placement in high scores
        } else {
            for (var i = scoreContent.highscores.length - 1; i >= 0; i--) {
                if ((msg.score <= scoreContent.highscores[i].score) || 
                    (i === 0)) {
                    if (i === scoreContent.highscores.length - 1) {
                        scoreContent.highscores.push({
                            "initials": msg.initials, 
                            "score": msg.score
                        });
                    } else {
                        scoreContent.highscores.splice(i + 1, 0, {
                            "initials": msg.initials, 
                            "score": msg.score
                        });
                    }
                    console.log(scoreContent.highscores);
                    scoreContent.highscores.splice(-1, 1);
                    break;
                }
            }
        }
        console.log(scoreContent.highscores);
        
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
    */
});

// Start server
console.log(sitePath);
console.log("Starting server in: " + __dirname + '/' + sitePath);
app.use(express.static(__dirname + '/' + sitePath));
http.listen(port, function() {
    console.log("Server running at: http://localhost:" + port);
});
