
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
	window.scrollTo(0, document.body.scrollHeight);
}

/* A query has finishd */
function qb_endquery(id) {
	window.scrollTo(0, document.body.scrollHeight);
}

