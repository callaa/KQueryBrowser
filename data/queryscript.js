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
	qbrowser.showBigResult(link.attributes['data-index']);
	return false;
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

