var socket = io.connect('http://paseolights.local:80');

function sendCMD(cmd){
    alert("oi");
    socket.emit('command', cmd);
}

$(document).on("click", ".click-led", function() {
    var red=$("#red").slider("value");
    var green=$("#green").slider("value");
    var blue=$("#blue").slider("value");
    sendCMD($(this).val());
    setTimeout(sendCMD(red), 5000);
    setTimeout(sendCMD(green), 10000);
    setTimeout(sendCMD(blue), 12000);
});
