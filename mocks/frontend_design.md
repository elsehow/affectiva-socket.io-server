# Streaming face-emotion low-level client

the low-level client is a js module, which can be installed via npm

## installation

    npm install streaming-faces

## usage

```javascript
var streaming = require('streaming-faces')

var stream = streaming(15000, 'http://my-streaming-faces-backend')
```

## API

### streaming(capture_time_ms, backend_uri)

returns an object `stream`

### stream.cam
a webcam stream, which we can pipe video out of (e.g. for real-time face display)

### stream.stop()

stop capture, close stream


# template front end (demo wraps low-level api) 

this is a demo that wraps the low-level API


Video features
	- Live webcam feed for user
	- Button to begin recording

User input
	- Video length
		- Buttons for preset lengths (10, 15, 20, 25, 30)
		- Custom input length
	- Video resolution

Display emotion data?

Make the site pretty
	- Landing page
		- Name
		- Live webcam feed
		- Record button
		- Video length input
	- About page
		- What it is about
		- What we used to build it
		- Why
		- BioSENSE group info
		- Link to GitHub?
	- Bootstrap?
