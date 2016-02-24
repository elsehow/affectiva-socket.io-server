var test = require('tape'),
    argv = require('minimist')(process.argv.slice(2)),
    exec = require('child_process').exec

console.log('bin to execute', argv.b)

function checkCLIOutput (t, cliBinPath) {
    exec('./' + cliBinPath, (err, stdout, stderr) => {
        // should call the executable correctly
        t.notOk(err, 'should be no error in calling')
        t.notOk(stderr, 'should be no error from process\'s stdout')
        t.ok(stdout, 'result should exist')
        // executable's stdout should be parseable as json
        var j = JSON.parse(stdout)
        t.ok(j, 'result should be valid json')
        // check it has some key fields
        t.ok(j[0], 'result should be a list')
        t.ok(j[0].expressions, 'object should have expressions field')
        t.ok(j[0].emotions, 'object should have emotions field')
        t.equals(0, j[0].timestamp, 'object should have timestamps field, where first item is 0')
        t.ok(j[0]['date and time'], 'object should have \'date-and-time\' field')
        // we're done
        t.end()
    })
}

test('test affectiva specs on shell command' + argv.s, t => {
  checkCLIOutput(t, argv.b)
})
