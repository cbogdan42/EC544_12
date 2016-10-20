var fs = require("fs");
var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('../data_from_sensor.db');
var Particle = require('particle-api-js');
var particle = new Particle();


var str = '\0';                   
var acc = 0;
var old = [];
var timeout_val = 20000;                      //Timeout after 20s
var timeout_check_time = [];
var avg = 0;


var file_path = '../data_server/public/files/file.txt';
var json_path = 'out.txt';
var device_list_path = 'device_list.txt';
var variable_file_path = '../bash_script/'
var variable_file_name = ['P1.txt','P2.txt','P3.txt','P4.txt'];
var photon_val = [];


var num_sensors = 3;                      //Constant determining number of sensors in the system

fs.writeFile(file_path, 'Sensor_id\tData_received\tTime\tDate', (err) => {
  if (err) throw err;
});


function init()                     //Function to initialize array to hold latest values from sensors
{ 
  var i;
  for(i=0;i<=num_sensors;i++)
  {
    photon_val[i] = 0;
    old[i] = 0;
    timeout_check_time[i] = 0;
  }
}


function json_parse(variable_json)
{
  var val = 10;
  return val;
}

function calculate_time(){                //Calculate the time periodically and return it
  var date_local = new Date();
  var hour = date_local.getHours();
  var time;
  var minutes = date_local.getMinutes();
  var seconds = date_local.getSeconds();
  time = ''
  time = hour.toString() + minutes.toString() + seconds.toString();
  return time;
}

function calculate_date(){                               
  var date_local = new Date();
  var date;
  var month = date_local.getMonth() + 1 
  var day = date_local.getDate() 
  var year = date_local.getFullYear() 
  date = ''
  date = year.toString() + month.toString() + day.toString();
  return date;
}


function print_avg(avg)                      //Print average value of temperature
{
  console.log("average computed = %dºC", avg.toFixed(2));
}

function check_source(device_id, msg)                //Check source of message
{
  
  var source;
  var i;
  var len;

  for(i=0;i<source_table.length;i++)
  {
    if(device_id == source_table[i])
      source = i;
  }
  
  //Record time of receiving message
  timeout_check_time[source] = Date.now();

  //Call function to calculate average
  compute_avg(msg,source);
} 


function compute_avg(msg, source)          //Calculate average based on the lastes 4 values from 'old' array
{
  var time;
  var date;
  var string_to_write = '\0'
  
  //Write to db
  time = calculate_time();
  date = calculate_date();
  write_to_db(source,msg,time,date)
  string_to_write = source + '\t' + msg.toFixed(2) + '\t' + time + '\t' + date + '\n'
  fs.appendFile(file_path, string_to_write, (err) => {
  if (err) throw err;
  });

  //Calculate average based on input data
  acc = acc + msg;
  
  //Shift array containing latest 4 values to accomodate new value
  shift_array(old,msg);
  acc = acc-old[0];
  avg = acc/num_sensors;
}

function check_for_timeout()              //Check which sensors haven't responded for more than 20s
{
  var i;
  for(i=0;i<num_sensors;i++)
  {
    if(Date.now()-timeout_check_time[i]>timeout_val)
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

function retrieve(device_id)       //Replace by particle_variable
{

  fs.readFile(variable_file_name[device_id], 'utf8', function(err, contents) {
    var str = JSON.parse(contents);
    photon_val[device_id] = parseFloat(str.result,10);
    compute_avg(photon_val[device_id],device_id);
  });
}


//Initialize array to hold latest values from sensors
init();

//Check if any sensors have timed out
//setInterval(check_for_timeout,5000);     

//setInterval(retrieve,2000);     //Call get_values here itself
setInterval(function(){retrieve(0)}, 1500);
setInterval(function(){retrieve(1)}, 1500);
setInterval(function(){retrieve(2)}, 1500);
setInterval(function(){print_avg(avg)}, 2000);

//setInterval(function(){retrieve(2)}, 1500);
//setInterval(function(){retrieve(3)}, 1500);