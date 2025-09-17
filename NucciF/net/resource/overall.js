function nano(template, data) {
	return template.replace(/\{([\w\.]*)\}/g, function(str, key) {
	var keys = key.split("."), v = data[keys.shift()];
	for (var i = 0, l = keys.length; i < l; i++) v = v[keys[i]];
	return (typeof v !== "undefined" && v !== null) ? v : "";
	});
}

function updateTable(query, data) {
	var table = document.querySelector(query);
	table.innerHTML = '';
	for(var key in data) {
		table.innerHTML += nano('<tr><td>{name}</td><td>{val}</td></tr>', data[key]);
	}
}

function updateInfo() {

	var oReq = new XMLHttpRequest();
	oReq.open('GET', 'statemeastask.bin', true);
	oReq.timeout = 5000;
	oReq.responseType = 'arraybuffer';
	
	oReq.ontimeout = function (oEvent) {
		var statusbox = document.querySelector('.statusbox');
		if(statusbox.classList.contains('hide')){
			statusbox.style.background = 'red';
			statusbox.innerHTML = 'Connection timeout';
			statusbox.classList.remove('hide');
		}
	}
	
	oReq.onload = function (oEvent) {
		if(oReq.status == 200){
			var statusbox = document.querySelector('.statusbox');
			if(!statusbox.classList.contains('hide')){
				statusbox.style.background = '#398a1e';
				statusbox.style.display = 'block';
				statusbox.innerHTML = 'Connection successful';
				setTimeout(function() {
					statusbox.classList.add('hide');
				}, 5000);
			}
		}

		var x = new DataView(oReq.response, 0);

		// Connect
		var connect = x.getUint8(0, true);

		// Enable
		var enable = x.getUint8(1, true);

		function round(x, dig) { return Number.parseFloat(x).toFixed(dig); }

		// Version
		major = x.getUint16(0, true);
		minor = x.getUint16(2, true);
		patch = x.getUint16(4, true);

		// Meas
		updateTable('.table2', [
			{name: 'Tube Voltage', val: x.getUint16(6, true) + ' V'},
			{name: 'Count Time', val: x.getUint16(8, true) + ' s'},
			{name: 'Radiation Val', val: round(x.getUint32(12, true) / 10.0, 1) + ' uR/h'},
			{name: 'Radiation', val: round(x.getUint32(16, true) / 10.0, 1) + ' uR'},			
		]);

		document.querySelector('footer').innerHTML = '©copyright: 2025 DEL	v' + major + '.' + minor + '.' + patch;
	};
	oReq.send(null);
}
updateInfo();
setInterval(updateInfo, 500);
