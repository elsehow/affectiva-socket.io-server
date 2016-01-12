// split script here
// takes
//
//    -i [input file] -l [input file length (ms)] -c [chunk size (ms)]
// 
// splits input file into chunks of length `chunk size`
// outputting files with the name
//
//    [input-file-name]-i.webm
//
// where i is the chunk number

var _ = require('lodash')
  , execSync = require('child_process').execSync

// returns a list of tuples
//   [ [start-time-ms, end-time-ms] ... ]
function time_chunks (file_length_ms, chunk_size_ms) {
  var chunks = _(_.range(file_length_ms))
                .chunk(chunk_size_ms)
                .value()
  
  var start_times = chunks.map(_.first)
  var end_times = chunks.map(_.last)
  
  return _.zip(start_times, end_times)
}

// takes a number of ms, e.g. 5000
// and returns a string like HH:MM:SS
//
//    stringify_ms(5000)
//    > 00:00:05 
function stringify_ms (ms) {
    return Math.round(ms / 1000)
}
//  function leading_zero (s) {
//    return s < 10 ? '0'+ s : s
//  }
//  function convert (s, eq) {
//    return Math.floor(s / 3600)
//  }
//  // s instead of ms
//  var total_seconds = Math.round(ms / 1000)
//  // hours
//  var hours = leading_zero(convert(total_seconds,  3600))
//  total_seconds %= 3600;
//  var minutes = leading_zero(convert(total_seconds, 60))
//
//  var seconds = leading_zero(total_seconds % 60);
//
//  return hours +':'+ minutes +':'+ seconds

// takes a tuple [start_time_ms, end_time_ms],
// a chunk index,
// and an input file name.
// returns an avconv command (string).
function make_command (start_end_tuple, chunk_i, in_file) {
    var string_start = stringify_ms(start_end_tuple[0])
    var string_end = stringify_ms(start_end_tuple[1])
    var fn_wo_ext = in_file.split('.webm')[0]
    return 'avconv ' +
        ' -ss ' + string_start +
        ' -t ' + string_end + 
        ' -i ' + in_file +
        ' -codec: copy ' + fn_wo_ext + '-' + chunk_i + '.webm'
}

var file_length = 10000
var chunk_size = 5000
var input_file = 'corpus/1.webm'

var chunks = time_chunks(file_length, chunk_size)

chunks.forEach((c, i) => {
    execSync(make_command(c, i, input_file))
    //console.log(make_command(c, i, input_file))
    console.log('done with', i, 'of', c.length)
})
