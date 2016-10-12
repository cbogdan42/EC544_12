webiopi().ready(function() {
    var STATE = function(macro, args, response) {
                    var state = response.split("");
                    // Following lines use jQuery functions
                    console.log("state:"+state)
        }
    webiopi().setFunction(17,"out");

    var content, button;
    content = $("#content");

    button = webiopi().createButton("button", "ON", function() {
        webiopi().callMacro("ON")
        })
    content.append(button);

    button = webiopi().createButton("button", "OFF", function() {
        webiopi().callMacro("OFF")
        })
    content.append(button);

    button = webiopi().createButton("button", "RAVE", function() {
        webiopi().callMacro("RAVE")
        })
    content.append(button);

    button = webiopi().createButton("button", "Check Value", function(){
        webiopi().callMacro("Get_Value",[],STATE)
        })
    content.append(button);

 
  });
