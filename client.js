var getUserMedia = require('get-user-media')
  , RecordRTC = require('recordrtc')

// options

// num of ms each clip will be
var clipDuration = 120000
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
// global mutable var
var num_recorded = 0

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
      // dl file
      num_recorded+= 1
      recordRTC.save(num_recorded+'.webm')
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

