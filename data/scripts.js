function loadFile(filePath) {
	let result = null;
	let xmlhttp = new XMLHttpRequest();

	xmlhttp.open("GET", filePath, false);
	xmlhttp.send();

	if (xmlhttp.status == 200) {
		result = xmlhttp.responseText;
	}

	return result;
}

function updateData() {
	let valuesLocal = loadFile("database.txt").split("\n");

	if (valuesLocal.length > totalLength) {
		let history = document.getElementById("history");

		for (let i = totalLength; i < valuesLocal.length; i++) {
			let entry = document.createElement('li');
			entry.innerHTML = valuesLocal[i];
			history.appendChild(entry);
		}

		totalLength = valuesLocal.length;
	}
}

var totalLength = 0;
setInterval(updateData, 0);
