var chunky = require('chunky-webcam')
  , getUserMedia = require('get-user-media')
  , attachMediaStream = require('attachmediastream')
  // element in which we show video
  , video_el = document.getElementById('my-video')
  // element in which we show server messages
  , msg_el = document.getElementById('server-messages')
  , ws_api_endpoint = 'localhost:3333'


// pretty print html div
function jsonDiv (obj) {
  return `<pre>${JSON.stringify(obj,null,4)}</pre>`
}

// get user webcam
getUserMedia({ video: true, audio: false}, (err, stream) => {

    // if the browser doesn't support user media 
    // or the user says "no" the error gets passed 
    // as the first argument. 
    if (err)
        console.log('error getting your webcam!', err);

    // put the webcam stream in a video element
    attachMediaStream(stream,video_el, {
        muted: true,
        mirror: true,
    })

    // setup chunky-webcam, with our stream
    // it will record 1000ms chunks,
    // and send them with a 'video' event to 'localhost:9999'
    // where our server runs
    chunk = chunky(stream, 5000, ws_api_endpoint)

    // our server will send 'data' events
    chunk.socket.on('data', data => {
        // we'll show the data in msg_el
        msg_el.innerHTML = jsonDiv(data)
    })

    chunk.socket.on('video-error', err => {
        msg_el.innerHTML = jsonDiv(err)
    })
})
