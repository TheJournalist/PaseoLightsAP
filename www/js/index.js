var socket = io.connect('http://paseolights.local:80');

$(document).on("click", ".click-led", function() {
    socket.emit('command', $(this).val());
});
