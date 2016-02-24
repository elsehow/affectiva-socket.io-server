"use strict";
var test = require('tape'),
    spawn = require('child_process').spawn
// name our server binary
var server_cmd = './server.js'
// we'll just use the mock cli client ofr now
var affectiva_exe = 'affectiva-cli-mock.js'

test('should require an executable -b from cli', t => {
    var proc = spawn(server_cmd)
    proc.stderr.on('data', e => {
      t.ok(e, 'should complain if i don\'t pass an executable -b to it')
      t.end()
    })
})

test('a post request should get our mock json as a response', t => {
  function doTestRequest () {
    var request = require('request')
    // post a request to the endpoint
    request.post('http://localhost:8889', (err, res, body) => {
      // no errors
      t.notOk(err, 'should be no error')
      // response has a body
      t.ok(res, 'response exists')
      // 200 response
      t.equal(200, res.statusCode, '200 ok')
      // content-type headers are json
      t.equal(res.toJSON().headers['content-type'], 'application/json', 'application/json content headers')
      t.ok(JSON.parse(res.body)[0].emotions, 'response is parseable affectiva data, and seems to have the right schema.')
      proc.kill()
      t.end()
    })
  }
  // spawn [node server.js] -b [affectiva_exe]
  var proc = spawn(server_cmd, ['-b', affectiva_exe])
  // when the server says it's listening, do the test request
  proc.stdout.on('data', doTestRequest)
})
