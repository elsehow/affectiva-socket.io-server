var app = require('http').createServer()
var io = require('socket.io')(app);
var fs = require('fs');
var uuid = require('uuid')
var execFile = require('child_process').execFile

app.listen(3333);

io.on('connection', (socket) => {
  // when we get a 'video' event, it contains a binary blob `blob`
  socket.on('video', blob => {
    // make a filename for the .webm
    var fn = `${uuid.v1()}.webm`
    // write the binary blob to disk
    fs.writeFile(fn, blob, (err, res) => {
      // after the file is written, exec emotions.exe with the filename
      var child = execFile('emotions.exe', ['video', fn], (error, stdout, stderr) => {
        // TODO stderr: is it a thing?
        // if theres an error, return as err
        if (error) socket.emit('err', error)
        // no error, send data
        else socket.emit('data', stdout)
        // delete the file now
        fs.unlink(fn) 
     })
    })
  });
});
