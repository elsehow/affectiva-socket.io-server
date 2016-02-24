var express = require('express')
  , app = express()
  , cors = require('cors')
  , bodyParser = require('body-parser')
  , uuid = require('uuid')
  , fs = require('fs')
  , path = require('path')

app.use(cors())
var jsonParser = bodyParser.json()
 
app.post('/', jsonParser, function (req, res) {
  var fn = path.join(__dirname, 'vids', `${uuid.v1()}.webm`)
  fs.writeFile(fn, JSON.parse(req.body), (err, res) => {
    if (err) console.log('err!', err)
    else console.log('wrote', name)
  })
})
 
app.listen(3000)
