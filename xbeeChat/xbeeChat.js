var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var acc = 0;
var avg = 0;
var old = new Array(0,0,0,0,0);
var count = 0;
var portName = process.argv[2],
portConfig = {
  baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};
var sp;
sp = new SerialPort.SerialPort(portName, portConfig);


function shift_array(arr,new_data)
{
  var i;
  var len;
  var j = 0;
  len = arr.length;
  //console.log(new_data);
  for(i=1;i<len;i++)
  { 
    arr[j] = arr[i];  
    j++;
  }
  arr[4] = new_data;
  //console.log(arr[0]);
}

function check_source(msg)
{
  var source;
  var data_char;
  var i;
  var local_data = 0;
  //console.log(msg); 
  var len = msg.length;
  //console.log(len);
  for(i=len-2;i>=0;i--)
  {
    
    data_char = msg.charCodeAt(i);
    data_char = data_char-48;
    //console.log(data_char);
    local_data = data_char*(Math.pow(10,len-i-2)) + local_data;

  }
  console.log(local_data);
  acc = acc + local_data;
  shift_array(old,local_data);
  acc = acc-old[0];
  avg = acc/4;
  count++;
  console.log("Average on %d reception is %f",count,avg);
}


app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {
    console.log('data received: ' + data);
    io.emit("chat message", "An XBee says: " + data);
    check_source(data);
  });
});
