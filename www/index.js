var socket = io.connect('http://paseolights.local:80');

function sendCMD(cmd){
    socket.emit('command', cmd);
}

$(document).on("click", ".click-led", function() {
    var red=$("#red").slider("value");
    var green=$("#green").slider("value");
    var blue=$("#blue").slider("value");
    sendCMD($(this).val());
    setTimeout(sendCMD(red), 15);
    setTimeout(sendCMD(green), 30);
    setTimeout(sendCMD(blue), 45);
});
