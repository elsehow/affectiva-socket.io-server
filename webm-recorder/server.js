var http = require('http');
 
var ecstatic = require('ecstatic')(__dirname + '/dist');
 
var server = http.createServer(ecstatic);

server.listen(9999);
console.log('server listening on http://localhost:9999')
