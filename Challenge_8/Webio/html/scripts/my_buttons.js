webiopi().ready(function() {
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

    button = webiopi().createButton("button", "FORWARD", function() {
        webiopi().callMacro("FORWARD")
        })
    content.append(button);

    button = webiopi().createButton("button", "BACKWARD", function() {
        webiopi().callMacro("BACKWARD")
        })
    content.append(button);

    button = webiopi().createButton("button", "LEFT", function() {
        webiopi().callMacro("LEFT")
        })
    content.append(button);

    button = webiopi().createButton("button", "RIGHT", function() {
        webiopi().callMacro("RIGHT")
        })
    content.append(button);
    
  });
