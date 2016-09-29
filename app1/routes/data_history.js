var express = require('express');
var router = express.Router();


/* GET home page. */
router.get('/', function(req, res, next) {
  //res.render(__dirname + '/hello.html');
  //console.log(__dirname)
  res.sendfile(__dirname + '/html/table_dynamic.html');
});

module.exports = router;
