var socket = io.connect('http://paseolights.local:80');

function sendColors(r, g, b){
    sendCMD(r);
    setTimeout(sendCMD(g), 15);
    setTimeout(sendCMD(b), 30);
}

function sendCMD(cmd){
    socket.emit('command', cmd);
}

$(document).on("click", ".click-led", function() {
    sendCMD($(this).val());
});
