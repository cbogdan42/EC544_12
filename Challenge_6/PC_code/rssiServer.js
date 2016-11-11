var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');
var rssi = [];
var final_rssi = [];
var count = 0;
var avg_count = 5;
var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});
var fs = require("fs");
var region = 1;
var portName = process.argv[2];

var sampleDelay = 1000;

var file_path = 'RSSI_data.txt';

function write_to_file()
{
  var string_to_write = region+'\t'+rssi[0]+'\t'+rssi[1]+'\t'+rssi[2] + '\t' + rssi[3] + '\n';
  fs.appendFile(file_path, string_to_write, (err) => {
  if (err) throw err;
  });  
}

for(var i=0;i<4;i++)
{
	final_rssi[i] = 0;
	count[i] = 0;
	rssi[i] = 0;
}

/*fs.writeFile(file_path, 'Region\txbee1\txbee2\txbee3\txbee4', (err) => {
  if (err) throw err;
});*/

//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);


//Create a packet to be sent to all other XBEE units on the PAN.
// The value of 'data' is meaningless, for now.
var RSSIRequestPacket = {
  type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
  destination64: "000000000000ffff",
  broadcastRadius: 0x01,
  options: 0x00,
  data: "test"
}

var requestRSSI = function(){
  sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

sp.on("open", function () {
  console.log('open');
  requestRSSI();
  setInterval(requestRSSI, sampleDelay);
});

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144){
    //console.log("inside");
  	var beacon_id = parseInt(frame.data[1]);
  	var val = parseInt(frame.data[0]); 
  	//console.log(beacon_id)
  	rssi[beacon_id-1] = val;
  	count++;
  	if(count == 4)
  	{
  		count = 0;
  		var string_to_write = rssi[0] +  '\t' + rssi[1] +  '\t' + rssi[2] +  '\t' + rssi[3];
	    console.log(string_to_write);
	    fs.writeFile(file_path, string_to_write, (err) => {
	      if (err) throw err;
	    });
  	}
  }
});
//setInterval(write_to_file,15000);