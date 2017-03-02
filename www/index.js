var socket = io.connect('http://paseolights.local:80');

$(document).on("click", ".click-led", function() {
    var red=$("#red").slider("value");
    var green=$("#green").slider("value");
    var blue=$("#blue").slider("value");
    socket.emit('command', $(this).val(), red, green, blue);
});
