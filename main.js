// Parameters
var sitePath = "www";
var port = 80;

// Libraries
var mraa = require('mraa');
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
D0.write(false);

var D1 = new m.Gpio(32); 
D1.dir(m.DIR_OUT);
D1.write(false);

var D2 = new m.Gpio(46); 
D2.dir(m.DIR_OUT);
D2.write(false);

var D3 = new m.Gpio(33); 
D3.dir(m.DIR_OUT);
D3.write(false);

var D4 = new m.Gpio(47); 
D4.dir(m.DIR_OUT);
D4.write(false);

var D5 = new m.Gpio(48); 
D5.dir(m.DIR_OUT);
D5.write(false);

var D6 = new m.Gpio(36); 
D6.dir(m.DIR_OUT);
D6.write(false);

var D7 = new m.Gpio(14); 
D7.dir(m.DIR_OUT);
D7.write(false);

var CLK = new m.Gpio(31); 
CLK.dir(m.DIR_OUT);
CLK.write(false);

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
  
  CLK.write(true);
  setTimeout(fall,100); //call the indicated function after 1 second (1000 milliseconds)
}

function fall()
{
  CLK.write(false);
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
sendCMD(0x04);
