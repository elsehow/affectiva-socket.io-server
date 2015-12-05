var shoe = require('shoe');
var http = require('http');
 
var ecstatic = require('ecstatic')(__dirname + '/dist');
 
var server = http.createServer(ecstatic);

var sock = shoe(function (stream) {
  stream.on('data', (b) => {
    console.log('got a .webm blob!!', b)
  })
});
sock.install(server, '/blobs');

server.listen(9999);
console.log('server listening on http://localhost:9999')
