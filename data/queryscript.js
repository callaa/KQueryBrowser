
/* Initialize the query browser. This is called when the browser has been loaded */
function qb_init() {
/*
	var queryform = document.createElement("form");
	var querybox = document.createElement("input");
	querybox.id = "query";
	queryform.appendChild(querybox);
	document.body.appendChild(queryform);

	queryform.onsubmit = function() {
		qbrowser.performQuery(querybox.value);
		querybox.value = "";
		return false;
	}
	*/
}

/* A new query was started */
function qb_newquery(id) {
	queryAbort();
	window.scrollTo(0, document.body.scrollHeight);
}

/* A query has reached a limit. More rows are available */
function qb_partialquery(id, got, expected) {
	var ui = document.getElementById("more-results");
	if(ui==null) {
		var qdiv = document.getElementById(id);
		var ui = document.createElement("div");
		ui.id = "more-results"
		ui.className = "ui";
		ui.innerHTML = '<a href="javascript:queryAbort()">Stop</a> <a href="javascript:qbrowser.queryGetMore()">Get more</a> <a href="javascript:qbrowser.queryGetAll()">Get the rest</a> <span class="status"></span>';
		qdiv.appendChild(ui);
	}
	if(expected>0) {
		ui.querySelector(".status").textContent = " (" + got + "/" + expected + ")";
	}
	window.scrollTo(0, document.body.scrollHeight);
}

/* A query has finishd */
function qb_endquery(id, total) {
	if(total>=0) {
		var summary = document.createElement("p");
		if(total==1) {
			summary.textContent = "Query returned one row.";
		} else {
			summary.textContent = "Query returned " + total + " rows.";
		}
		document.querySelector("div.query:last-child").appendChild(summary);
	}
	var btns = document.getElementById("more-results");
	if(btns!=null) {
		btns.parentNode.removeChild(btns);
	}

	window.scrollTo(0, document.body.scrollHeight);
}

function queryAbort() {
	var btns = document.getElementById("more-results");
	if(btns!=null) {
		var count = btns.parentNode.querySelector("tbody").childElementCount;
		var summary = document.createElement("p");
		if(count==1) {
			summary.textContent = "Query aborted after one row.";
		} else {
			summary.textContent = "Query aborted after " + count + " rows.";
		}
		btns.parentNode.appendChild(summary);
		btns.parentNode.removeChild(btns);
	}
}

