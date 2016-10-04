var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var fs = require("fs");
var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('data_from_sensor.db');

var str = '\0';										
var acc = 0;
var avg = 0;
var old = [];
var timeout_val = 20000;                      //Timeout after 20s
var data_time = [];

var file_path = '../data_server/public/files/file.txt';

var num_sensors = 3;                      //Constant determining number of sensors in the system


fs.writeFile(file_name, 'Sensor_id\tData_received\tTime\tDate', (err) => {
	if (err) throw err;
});


function init_array()                     //Function to initialize array to hold latest values from sensors
{ 
  var i;
  for(i=0;i<=num_sensors;i++)
  {
    old[i] = 0;
    data_time[i] = 0;
  }
}

function calculate_time(){								//Calculate the time periodically and store in variable time
  var date_local = new Date();
  var hour = date_local.getHours();
  var time;
  var minutes = date_local.getMinutes();
  var seconds = date_local.getSeconds();
  time = ''
  time = hour + ':' + minutes + ':' + seconds;
  return time;
}

function find_date(){
  var date_local = new Date();
  var date;
  var month = date_local.getMonth() + 1 
  var day = date_local.getDate() 
  var year = date_local.getFullYear() 
  date = ''
  date = month + '/' + day + '/' + year
  return date;
}


function print_avg()                      //Print average value of temperature
{
  console.log("average computed = %dºC", avg.toFixed(2));
}

function check_source(msg)                //Check source of message
{
  var source;
  var i;
  var len;

  //Find source of message
  len = msg.length;
  source =  msg.charCodeAt(0);
  source = source-49;  

  //Record time of receiving message
  data_time[source] = Date.now();

  //Call function to calculate average
  compute_avg(msg,len,source);
} 


function compute_avg(msg, len, source)          //Calculate average based on the lastes 4 values from 'old' array
{
  var num;
  var whole_num;
  var decimal_part;
  var local_data = 0;
  var str;
  var i;
  var time;
  var data;
  var string_to_write = '\0'
  //Convert string to number
  num = msg.slice(1,len-1);
  str = num.split(".");
  whole_num = str[0];
  decimal_part = str[1];
  local_data = (whole_num.charCodeAt(0)-48)*10 + whole_num.charCodeAt(1)-48 + (decimal_part.charCodeAt(0)-48)/10 + (decimal_part.charCodeAt(1)-48)/100;				//Float value containing sensor data

  //Write to db
  time = calculate_time();
  date = find_date();
  write_to_db(source,local_data,time,date)
  string_to_write = source + '\t' + local_data + '\t' + time + '\t' + date + '\n'
  fs.appendFile(file_path, string_to_write, (err) => {
	if (err) throw err;
  });

  //Calculate average based on input data
  acc = acc + local_data; 
  
  //Shift array containing latest 4 values to accomodate new value
  shift_array(old,local_data);
  acc = acc-old[0];
  avg = acc/num_sensors;
  print_avg();

}

function check_for_timeout()              //Check which sensors haven't responded for more than 20s
{
  var i;
  for(i=0;i<num_sensors;i++)
  {
    if(Date.now()-data_time[i]>timeout_val)
      console.log("Sensor %d hasn't responded for more than 20 seconds",i+1);
  } 
}
  

function shift_array(arr,new_data)          //Function used to shift the array to calculate average of latest 4 values
{
  var i;
  var len;
  var j = 0;
  len = arr.length;
  for(i=1;i<len;i++)
  { 
    arr[j] = arr[i];  
    j++;
  }
  arr[num_sensors] = new_data;
}


function write_to_db(source, value, time, date){
  //console.log("Inside");
  db.serialize(function() {
    db.run("CREATE TABLE IF NOT EXISTS sensor_data (sensor_id INTEGER, sensor_output FLOAT, time TEXT, date TEXT)");
    var stmt = db.prepare("INSERT INTO sensor_data VALUES(?,?,?,?)");

    stmt.run(source,value,time,date);

    stmt.finalize();
  });
}



//Initialize ports
var portName = process.argv[2],
portConfig = {
  baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};
var sp;
sp = new SerialPort.SerialPort(portName, portConfig);



//Initialize array to hold latest values from sensors
init_array();

//Check if any sensors have timed out
setInterval(check_for_timeout,10000);     


app.get('/', function(req, res){
  res.sendfile('index.html');
});


http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {
    //console.log('data received: ' + data);
    check_source(data);
  });
});