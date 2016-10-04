var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var fs = require("fs");
var file_path = '../data_server/public/files/file.txt';
//var file_name = ''

fs.writeFile(file_path, '', (err) => {
	if (err) throw err;
});


app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
  });
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    console.log(msg)
    console.log(file_path)
    fs.appendFile(file_path, msg + '\n', (err) => {
	if (err) throw err;
	});
  });
});


http.listen(3000, function(){
  console.log('listening on *:3000');
});