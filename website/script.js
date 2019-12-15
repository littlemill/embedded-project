const APPID = 'testingNetpie';
const KEY = 'Y6ynR78mqQBIlSj';
const SECRET = 'wX8up1DQXlQ413t3glkRj5ehI';

const ALIAS = 'DigitalOUTPUT_HTML_web'; //  ชื่อตัวเอง
const thing1 = 'NodeMCU1'; //  ชื่อเพื่อนที่จะคุย

var microgear = Microgear.create({
  key: KEY,
  secret: SECRET,
  alias: ALIAS
});

microgear.connect(APPID);

function falling(logic) {
  if (logic == 1) {
    microgear.publish('/fall/command', 'ON');
    document.getElementById('status_fall_btn').innerHTML = 'Status : ON';
  } else if (logic == 0) {
    microgear.publish('/fall/command', 'OFF');
    document.getElementById('status_fall_btn').innerHTML = 'Status : ON';
  } else if (logic == 2) {
    microgear.publish('/fall/command', 'RESET');
  }
}

function goOut(logic) {
  if (logic == 1) {
    console.log('logic1 = on');
    microgear.publish('/out-of-range/command', 'ON');
    document.getElementById('status_range_btn').innerHTML = 'Status : ON';
  } else if (logic == 0) {
    console.log('logic0 = off');
    microgear.publish('/out-of-range/command', 'OFF');
    document.getElementById('status_range_btn').innerHTML = 'Status : OFF';
  } else if (logic == 2) {
    console.log('logic2 = reset');
    microgear.publish('/out-of-range/command', 'RESET');
  }
}

microgear.on('connected', function() {
  console.log('NETPIE Connected');
  microgear.setAlias(ALIAS);
  document.getElementById('connected_NETPIE').innerHTML =
    'netpie status : connected !';
});

microgear.on('message', function(topic, data) {
  if (topic == '/fall') {
    if (data == 'ok') {
      document.getElementById('status_fall').innerHTML = 'Every is OKAY :)';
    } else if (data == 'fall') {
      document.getElementById('status_fall').innerHTML = 'Falling!!!!!!!!!';
    }
  } else if (topic == '/range') {
    if (data == 'in') {
      document.getElementById('status_fall').innerHTML = 'I can see you';
    } else if (data == 'out') {
      document.getElementById('status_fall').innerHTML = 'where are youuu';
    }
  } else if (topic == '/board') {
    if (data == 'fine') {
      document.getElementById('status_fall').innerHTML = 'use me XD';
    } else if (data == 'error') {
      document.getElementById('status_fall').innerHTML = 'fix me XP';
    }
  }
});

microgear.on('present', function(event) {
  console.log(event);
});

microgear.on('absent', function(event) {
  console.log(event);
});

// microgear.resettoken(function(err) {
//   microgear.connect(APPID);
// });
