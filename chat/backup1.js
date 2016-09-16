var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var f = "log.txt"
var number_of_data_chars = 5;
var n;
var acc = 0;
var avg = 0;
var old = new Array(0,0,0,0,0);

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
	var len = length.msg
	for(i=0;i<3;i++)
	{
		
		data_char = msg.charCodeAt(i);
		data_char = data_char-48;
		if(i==0)
			local_data = data_char*100+local_data;
		else if(i==1)
			local_data = data_char*10+local_data;
		else
			local_data = data_char+local_data;

	}
	//console.log(local_data);
	acc = acc + local_data;
	shift_array(old,local_data);
	acc = acc-old[0];
	avg = acc/4;
	console.log(avg);
}

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg)
{
    io.emit('chat message', msg);
    check_source(msg);
});
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});
