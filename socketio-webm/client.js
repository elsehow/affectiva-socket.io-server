var getUserMedia = require('get-user-media')
  , RecordRTC = require('recordrtc')
  , io = require('socket.io-client')
  , socket = io('localhost:3333')

socket.on('err', err => console.log('err', err))

socket.on('data', data => console.log('data', data))

// utility fn for posting vid data
function postVideo (blob) {
  socket.emit('video', blob)
}

// options

// num of ms each clip will be (ms)
var clipDuration = 10000
// options for recording stuff
var recOptions = {
  type: 'video',
  mimeType: 'video/webm',
  video: {
    width: 320,
    height: 240,
  },
  frameInterval: 10,
  disableLogs: true,
}

// logic

function recordASec (err, stream) {
  if (err) throw err
  var recordRTC = RecordRTC(stream, recOptions)
  // start it recording
  recordRTC
    .startRecording()
    .setRecordingDuration(clipDuration)
    // when it stops,
    .onRecordingStopped(videoURL => {
      // post the video data
      var blob = recordRTC.getBlob()
      postVideo(blob)
      // start recording again
      recordASec(err, stream)
  })
}

// setup

// start recording
getUserMedia({ 
  video: true,
  audio: false,
  }, recordASec)

