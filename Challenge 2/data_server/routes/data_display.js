var express = require('express');
var router = express.Router();
var dir = 

/* GET home page. */
console.log(__dirname)
router.get('/', function(req, res, next) {
  res.sendfile(__dirname + '/html/data_display.html');
});

module.exports = router;
