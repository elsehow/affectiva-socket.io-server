#! /usr/local/bin/node

var spawn = require('child_process').spawn,
    server = require('express')(),
    port = 8889

var argv = require('minimist')(process.argv.slice(2))
var affectiva_cli_binary = argv.b

if (!affectiva_cli_binary) {
  process.stderr.write("ERR!!!! Pass a path to the affectiva cli binary with -b. e.g. node server -b my/affectiva-binary")
  process.exit(1)
}

server.post('/', (req, res) => {
  var proc = spawn(`./${affectiva_cli_binary}`)
  res.status(200)
  res.setHeader('Content-Type', 'application/json');
  proc.stdout.pipe(res)
})

server.listen(port, () => console.log(`listening on ${port}`))
