//Modules for running the page
var express = require('express');
var app = express();
var path = require('path');
var http = require('http').Server(app);
var io = require('socket.io')(http);


//Add routes here
var routes = require('./routes/index');



//Declaring the app object
app.use(express.static(__dirname + '/public'));
// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');


//location of different route js files
app.use('/', routes);

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    console.log(msg)
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

module.exports = app;
