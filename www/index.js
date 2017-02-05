var socket = io.connect('http://paseolights.local:80');

function sendColors(){
    sendCMD(getred);
    setTimeout(sendCMD(getgreen), 15);
    setTimeout(sendCMD(getblue), 30);
}

function sendCMD(cmd){
    socket.emit('command', cmd);
}

$(document).on("click", ".click-led", function() {
    sendCMD($(this).val());
});
