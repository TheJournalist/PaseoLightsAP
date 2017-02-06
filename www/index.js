var socket = io.connect('http://paseolights.local:80');

function sendCMD(cmd){
    socket.emit('command', cmd);
}

$(document).on("click", ".click-led", function() {
    var red=$("#red").slider("value");
    var green=$("#green").slider("value");
    var blue=$("#blue").slider("value");
    sendCMD($(this).val());
    
    setTimeout(function fred() {
    socket.emit('command', red);
    }, 50);
    
    setTimeout(function fgreen() {
    socket.emit('command', green);
    }, 100);
    
    setTimeout(function fblue() {
    socket.emit('command', blue);
    }, 150);
});
