var getUserMedia = require('get-user-media')
  , RecordRTC = require('recordrtc')
  , shoe = require('shoe')
  , stream = shoe('/blobs');

// num of ms each clip will be
var clipDuration = 5000
// options for recording stuff
var recOptions = {
  type: 'video',
  mimeType: 'video/webm',
  video: {
    width: 320,
    height: 240,
  },
  frameInterval: 10,
}

function sendBlob (b) {
  stream.write(b)
}

function recordASec (err, stream) {
  if (err) throw err
  var recordRTC = RecordRTC(stream, recOptions)
  // start it recording
  recordRTC
    .startRecording()
    .setRecordingDuration(clipDuration)
    // when it stops,
    .onRecordingStopped(videoURL => {
      // start recording again
      recordASec(err, stream)
      // send the blob (.webm file) to the server
      var blob = recordRTC.getBlob();
      sendBlob(blob)
  })
}
 
getUserMedia({ 
  video: true,
  audio: false,
  }, recordASec)

