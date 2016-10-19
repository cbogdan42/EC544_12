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
var source_table = []


var file_path = '../data_server/public/files/file.txt';

var num_sensors = 4;                      //Constant determining number of sensors in the system


fs.writeFile(file_path, 'Sensor_id\tData_received\tTime\tDate', (err) => {
	if (err) throw err;
});

particle.login({username: 'aravinds@bu.edu', password: 'ec544group12'}).then(
	//Function to initialize array to hold latest values from sensors
	function init_array()                    
	{ 
		var i =0;

		for(i=0;i<=num_sensors;i++)
		{
			old[i] = 0;
			timeout_check_time[i] = 0;
		}
		source_table = find_device_id();
	  		console.log(source_table+'hi')
	},
	
    function(data)
    {
        var devicesPr = particle.listDevices({ auth: data.body.access_token });
        devicesPr.then(
        function(devices)
        {
          var parsed_val = json_parse(devices.body);
          check_source(device_id, parsed_val);  //If getting value from one photon, do string parsing here
          var i;
          var source = [];
  		  for(i=0;i<=num_sensors;i++)
  		  {
    	  	old[i] = 0;
    		timeout_check_time[i] = 0;
  		  }
  		  source[0] = devices.body[0].id;
          source[1] = devices.body[1].id;
          console.log('in devices');
          //source[2] = devices.body[2].id;
          //source[3] = devices.body[3].id;
  		//source_table = find_device_id();		
		},

		function json_parse(variable_json)
		{
		  var stringed = JSON.stringify({ cmd: 'VarReturn',
		                                  name: 'sum',
		                                  result: 60,
		                                  coreInfo: 
		                                   { last_app: '',
		                                     last_heard: '2016-10-18T01:03:33.862Z',
		                                     connected: true,
		                                     last_handshake_at: '2016-10-18T01:01:35.866Z',
		                                     deviceID: '260039000a47353235303037',
		                                     product_id: 6 } });
		  var splitcomma = stringed.split(',');
		  var splitcolon = splitcomma[2].split(':');
		  var sum = parseFloat(splitcolon[1]);
		  console.log(sum);
		  //var val = 10;
		  return sum;
		},

		function calculate_time()
		{								//Calculate the time periodically and return it
  			var date_local = new Date();
  			var hour = date_local.getHours();
  			var time;
  			var minutes = date_local.getMinutes();
  			var seconds = date_local.getSeconds();
  			time = ''
  			time = hour +  minutes +  seconds;
 		    console.log(typeof(time))
  			return time;
  			console.log('in calc time');
        },

		function calculate_date(){                               
 			 var date_local = new Date();
 			 var date;
 			 var month = date_local.getMonth() + 1 
 			 var day = date_local.getDate() 
  			 var year = date_local.getFullYear() 
 			 date = ''
 			 date = year + month  + day
  			 console.log(typeof(date))
  			 return date;
		},

		function check_source(device_id, msg)                //Check source of message
		{
		  
		  var source;
		  var i;
		  var len;
		  console.log('in check source');
		  //Find source of message
		  //len = msg.length; 
		  for(i=0;i<source_table.length;i++)
		  {
		  	if(device_id == source_table[i])
		  		source = i;
		  }
		  
		  //Record time of receiving message
		  timeout_check_time[source] = Date.now();

		  //Call function to calculate average
		  compute_avg(msg,source);
		}, 

		function compute_avg(msg, source)          //Calculate average based on the lastes 4 values from 'old' array
		{
		  var time;
		  var date;
		  var avg = 0;
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
		  print_avg(avg);
		  console.log("average computed = %dºC", avg.toFixed(2));

		},

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
		},

		function check_for_timeout()              //Check which sensors haven't responded for more than 20s
		{
		  var i;
		  for(i=0;i<num_sensors;i++)
		  {
		    if(Date.now()-timeout_check_time[i]>timeout_val)
		      console.log("Sensor %d hasn't responded for more than 20 seconds",i+1);
		  } 
		},
// HOW to create database if scope of variables is limited to devices function???????????????????
		function write_to_db(source, value, time, date){
		  //console.log("Inside");
		  db.serialize(function() {
		    db.run("CREATE TABLE IF NOT EXISTS sensor_data (sensor_id INTEGER, sensor_output FLOAT, time TEXT, date TEXT)");
		    var stmt = db.prepare("INSERT INTO sensor_data VALUES(?,?,?,?)");

		    stmt.run(source,value,time,date);

		    stmt.finalize();
		  });
		  init_array();
		 },

        function(err) {
          console.log('List devices call failed: ', err);
        }
      );
    },
    function(err) {
      console.log('API call completed on promise fail: ', err);
    }
  );
