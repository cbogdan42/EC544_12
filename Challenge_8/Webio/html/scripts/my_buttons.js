webiopi().ready(function() {

    // Keyboard input code    
    document.onkeydown = function(event) {
        var key_press = String.fromCharCode(event.keyCode);
        var key_code = event.keyCode;

        document.getElementById('kp').innerHTML = key_press; 
        document.getElementById('kc').innerHTML = key_code;
        
        if(key_code == 37){
            webiopi().callMacro("LEFT");
        }
        else if(key_code == 39){
            webiopi().callMacro("RIGHT");
        }
        else if(key_code == 38){
            webiopi().callMacro("FORWARD");
        }
        else if(key_code == 40){
            webiopi().callMacro("BACKWARD");
        }
    }
    

    // Button Stuff
    var STATE = function(macro, args, respons) {
        var state = response.split("");
        // Following lines use jQuery functions
        // console.log("state:"+state)
        console.log("Hello World 1")
        }

    webiopi().setFunction(17,"out");

    var content, button;
    content = $("#content");

    button = webiopi().createButton("button", "MANUAL", function() {
        webiopi().callMacro("MANUAL");
        })
    content.append(button);

    button = webiopi().createButton("button", "AUTONOMOUS", function() {
        webiopi().callMacro("AUTONOMOUS")
        })
    content.append(button);

    button = webiopi().createButton("button", "AUTO - START", function() {
        webiopi().callMacro("ASTART")
        })
    content.append(button);

    button = webiopi().createButton("button", "AUTO - STOP", function() {
        webiopi().callMacro("ASTOP")
        })
    content.append(button);
    
  });
