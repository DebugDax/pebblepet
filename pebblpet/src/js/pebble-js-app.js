var vage;
var vhappy;
var vhealth;
var vtimeleft;
var vlevel;
var vtotal;
var cver = 1.0;
var tut;
var petsync;

//resetStats();

Pebble.addEventListener("ready", function(e) {					  
	loadGlobals();
	Pebble.sendAppMessage( { "age":JSON.parse(vage), "happiness":JSON.parse(vhappy), "health":JSON.parse(vhealth), "timeleft":JSON.parse(vtimeleft), "level":JSON.parse(vlevel), "totaltime":JSON.parse(vtotal)},
	 function(e) {
		console.log("Successfully delivered message");
		console.log("age" + JSON.parse(vage) + ", happiness" + JSON.parse(vhappy) + ", health" + JSON.parse(vhealth) + ", timeleft" + JSON.parse(vtimeleft) +  ", level" + JSON.parse(vlevel));
	  },
	  function(e) {
		console.log("Unable to deliver message.");
	  }
	);
});
						
Pebble.addEventListener("appmessage", function(e) {
		console.log("AppMessage recieved");
		console.log(JSON.stringify(e.payload));

		console.log("Stats saved!");
		if (e.payload.age != 'undefined') { localStorage.setItem("lsAge", JSON.stringify(e.payload.age)); }
		if (e.payload.happiness != 'undefined') { localStorage.setItem("lsHappy", JSON.stringify(e.payload.happiness)); }
		if (e.payload.health != 'undefined') { localStorage.setItem("lsHealth", JSON.stringify(e.payload.health)); }
		if (e.payload.timeleft != 'undefined') { localStorage.setItem("lsTime", JSON.stringify(e.payload.timeleft)); }
		if (e.payload.level != 'undefined') { localStorage.setItem("lsLevel", JSON.stringify(e.payload.level)); }
		if (e.payload.totaltime != 'undefined') { localStorage.setItem("lsTotal", JSON.stringify(e.payload.level)); }
		
		if (petsync >= 1) {
			loadGlobals();
			var serial = Pebble.getAccountToken();
			var req = new XMLHttpRequest();
			req.open('GET', 'http://domain.com/page.php?x=1&s='+serial+'&l='+vlevel+'&a='+vage, true);
			req.send(null);
		}
});

Pebble.addEventListener("showConfiguration", function(e) {
	loadGlobals();
	Pebble.openURL("http://domain.com/page.php?age="+vage+"&happiness="+vhappy+"&health="+vhealth+"&timeleft="+vtimeleft+"&level="+vlevel+"&ta="+vtotal+"&cver="+cver+"&petsync="+petsync+"&tut="+tut);
});

Pebble.addEventListener("webviewclosed", function(e) {
	if (e.response == "notrack") {
		localStorage.setItem("petsync", "0");		
	} else if (e.response == "track") {
		localStorage.setItem("petsync", "1");	
	} else if (e.response == "reset") {
		resetStats();
	} else if (e.response == "tut") {
		localStorage.setItem("tut", "1");
	}
});

function loadGlobals() {
	vage = localStorage.getItem("lsAge");
	vhappy = localStorage.getItem("lsHappy");
	vhealth = localStorage.getItem("lsHealth");
	vtimeleft = localStorage.getItem("lsTime");
	vlevel = localStorage.getItem("lsLevel");
	vtotal = localStorage.getItem("lsTotal");
	petsync = localStorage.getItem("petsync");
	tut = localStorage.getItem("tut");
	
	if (!vage || JSON.parse(vage) == 'undefined') { vage = 0; }
	if (!vhappy || JSON.parse(vhappy) == 'undefined') { vhappy = 100; }
	if (!vhealth || JSON.parse(vhealth) == 'undefined') { vhealth = 100; }
	if (!vtimeleft || JSON.parse(vtimeleft) == 'undefined') { vtimeleft = 30; }
	if (!vlevel || JSON.parse(vlevel) == 'undefined') { vlevel = 0; }
	if (!vtotal || JSON.parse(vtotal) == 'undefined') { vtotal = 0; }
	if (!petsync || JSON.parse(petsync) == 'undefined') { petsync = 0; }
	if (!tut || JSON.parse(tut) == 'undefined') { tut = 0; }
}

function resetStats() {
	localStorage.removeItem('lsAge');
	localStorage.removeItem('lsHappy');
	localStorage.removeItem('lsHealth');
	localStorage.removeItem('lsTime');
	localStorage.removeItem('lsLevel');
	localStorage.removeItem('lsTotal');
	localStorage.removeItem('petsync');
	localStorage.setItem("petsync", "1");	
}