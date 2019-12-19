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
    // console.log('logic1 = on');
    microgear.publish('/fall/command', 'ON');
    document.getElementById('status_fall_btn').innerHTML = 'Status : ON';
  } else if (logic == 0) {
    // console.log('logic0 = off');
    microgear.publish('/fall/command', 'OFF');
    document.getElementById('status_fall_btn').innerHTML = 'Status : OFF';
  } else if (logic == 2) {
    // console.log('logic2 = reset');
    microgear.publish('/fall/command', 'RESET');
    document.getElementById('status_fall').innerHTML = 'Everything is OKAY :)';
  }
}

function goOut(logic) {
  if (logic == 1) {
    // console.log('logic1 = on');
    microgear.publish('/out-of-range/command', 'ON');
    document.getElementById('status_range_btn').innerHTML = 'Status : ON';
  } else if (logic == 0) {
    // console.log('logic0 = off');
    microgear.publish('/out-of-range/command', 'OFF');
    document.getElementById('status_range_btn').innerHTML = 'Status : OFF';
  } else if (logic == 2) {
    // console.log('logic2 = reset');
    microgear.publish('/out-of-range/command', 'RESET');
    document.getElementById('status_fall').innerHTML = 'Everything is OKAY :)';
  }
}

microgear.on('message', function(topic, data) {
  console.log(topic);
  topic = topic.replace('/testingNetpie', '');

  if (topic == '/fall') {
    if (data == 'ok') {
      document.getElementById('status_fall').innerHTML =
        'Everything is OKAY :)';
    } else if (data == 'fall') {
      document.getElementById('status_fall').innerHTML =
        'The patient is falling!!!!!!!!!';
    }
  } else if (topic == '/range') {
    if (data == 'in') {
      document.getElementById('status_range').innerHTML =
        "Patient's location is available";
    } else if (data == 'out') {
      document.getElementById('status_range').innerHTML =
        "Patient's location is unavailable";
    }
  } else if (topic == '/board') {
    if (data == 'fine') {
      //document.getElementById('status_fall').innerHTML = 'use me XD';
    } else if (data == 'error') {
      document.getElementById('status_range').innerHTML = 'fix me XP';
    }
  } else if (topic == '/status/command') {
    if (data == 'fall_on') {
      document.getElementById('status_fall_btn').innerHTML = 'Status : ON';
    } else if (data == 'fall_off') {
      document.getElementById('status_fall_btn').innerHTML = 'Status : OFF';
    } else if (data == 'out_on') {
      document.getElementById('status_range_btn').innerHTML = 'Status : ON';
    } else if (data == 'out_off') {
      document.getElementById('status_range_btn').innerHTML = 'Status : OFF';
    }
  }
});

microgear.on('present', function(event) {
  console.log(event);
});

microgear.on('absent', function(event) {
  console.log(event);
});

microgear.on('connected', function() {
  console.log('NETPIE Connected');
  microgear.setAlias(ALIAS);
  document.getElementById('connected_NETPIE').innerHTML =
    'Netpie status : connected !';
  microgear.subscribe('/status/command');
  microgear.subscribe('/range');
  microgear.subscribe('/fall');
  microgear.subscribe('/board');
});

microgear.connect(APPID);
// microgear.resettoken(function(err) {
//   microgear.connect(APPID);
// });
