// Usage:  "node 56-echo_serv_TCP <port>"

var net = require('net');
var port = (process.argv[2] || 6060);

                         // Create server socket
var server = net.createServer(function(socket) {
	socket.write('CONNECTED TO SERVER');  // Send welcome
	socket.on('data', function(data) {
        //socket.write(data);           // Send echo
        console.log(data+"");
	});
});

server.listen(port);

console.log("TCP Echo server started at port: " + port);

