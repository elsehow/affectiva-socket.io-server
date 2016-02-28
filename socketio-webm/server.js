var app      = require('http').createServer()
  , io       = require('socket.io')(app)
  , fs       = require('fs')
  , uuid     = require('uuid')
  , execFile = require('child_process').execFile

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
<<<<<<< HEAD
        // if theres an error, return as err
        if (error) socket.emit('error', error)
        // no error, send data
=======
        // if theres an error, return as error
        if (error) socket.emit('error', error)
        // if theres a stderr, return as stderr
        else if (stderr) socket.emit('stderr', stderr)
        //no error, send date
>>>>>>> dd92df28f93329fe334a7e6981f2bbd74656801e
        else socket.emit('data', stdout)
        // delete the file now
        fs.unlink(fn) 
     })
    })
  });
});
