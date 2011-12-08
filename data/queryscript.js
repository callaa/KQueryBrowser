var EXPORTERS;
/* Initialize the query browser. This is called when the browser has been loaded */
function qb_init(exporters) {
	EXPORTERS=exporters;
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
		// Add buttons
		var qdiv = document.getElementById(id);
		var ui = document.createElement("div");
		ui.id = "more-results"
		ui.className = "ui";
		ui.innerHTML = '<button id="abortbtn">Stop</button> <button id="morebtn">Get more</button> <button id="allbtn">Get the rest</button> <span class="status"></span>';
		qdiv.appendChild(ui);
		document.getElementById("abortbtn").onclick = queryAbort;
		document.getElementById("morebtn").onclick = queryGet(qbrowser.queryGetMore);
		document.getElementById("allbtn").onclick = queryGet(qbrowser.queryGetAll);
	} else {
		// Re-enable buttons
		var btns = ui.querySelectorAll("button");
		for(var i=0;i<btns.length;++i) {
			btns[i].disabled = false;
		}
	}
	if(expected>0) {
		ui.querySelector(".status").textContent = " (" + got + "/" + expected + ")";
	}
	window.scrollTo(0, document.body.scrollHeight);
}

/* Abort a query */
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
	return false;
}

/* Show spinner and execute function to get more query results. This
 * function returns a button callback function that performs the action. */
function queryGet(fn) {
	return function() {
		// Add spinner
		var spinner = document.createElement("p");
		spinner.className="wait";
		this.parentNode.parentNode.appendChild(spinner);

		// Disable buttons
		var btns = this.parentNode.querySelectorAll("button");
		for(var i=0;i<btns.length;++i) {
			btns[i].disabled = true;
		}
		
		// Execute query function
		fn();
		return false;
	}
}

/* A query has finishd */
function qb_endquery(id, total) {
	if(total>=0) {
		var qdiv = document.querySelector("div.query:last-child");

		var summary = document.createElement("p");
		summary.className="summary";
		if(total==1) {
			summary.textContent = "Query returned one row.";
		} else {
			summary.textContent = "Query returned " + total + " rows.";
		}
		qdiv.appendChild(summary);

		if(total>0) {
			var ediv = document.createElement("div");
			ediv.className="export";
			var espan = document.createElement("span");
			espan.textContent="Export";
			ediv.appendChild(espan);

			for(var i=0;i<EXPORTERS.length;++i) {
				var e = EXPORTERS[i];
				var ee = document.createElement("a");
				ee.href = "#";
				ee.onclick = (function(format) {
					return function() { qbrowser.exportTable(id, format); return false; };
				})(e.format);

				if(e.icon!=null) {
					var img = document.createElement("img");
					img.src = e.icon;
					img.title = e.format;
					ee.appendChild(img);
				} else {
					ee.textContent = e.format;
				}
				ediv.appendChild(ee);
			}
			qdiv.appendChild(ediv);
		}
	}
	var btns = document.getElementById("more-results");
	if(btns!=null) {
		btns.parentNode.removeChild(btns);
	}
	window.scrollTo(0, document.body.scrollHeight);
}


/* Show a column's full value */
function qb_show(link) {
	qbrowser.showBigResult(link.getAttribute('data-index'));
	return false;
}

