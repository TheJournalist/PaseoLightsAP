/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

var socket = io.connect('http://paseolights.local:80');

$(document).on("click", ".click-led", function() {
    socket.emit('command', $(this).val());
});
