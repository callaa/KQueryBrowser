// Called when a new query has been started
function newQuery(kwargs) {
  const e = domElement;

  const el = e('h1', {'class': 'query'}, kwargs.sql);
  document.getElementById('content').appendChild(el);
  el.scrollIntoView();
}

// Called externally when new results are available
async function pollResults(resultId) {
  const response = await fetch(`api:getResults/${resultId}`, {
    method: 'POST',
    body: "{}"
  });

  const results = await response.json();

  if('rows' in results) {
    if('columns' in results) {
      addResultTable(resultId, results);
    } else {
      continueResultsTable(resultId, results);
    }

  } else if('error' in results) {
    addErrorMessage(results.error)
  }
}

// Called externally to request the result history to be cleared
function clearResults() {
  document.getElementById('content').innerHTML = '';
}

// Called externally to let know that the last query is now closed
function stopResults() {
  const existingResults = document.querySelectorAll('#content>.result');
  if(existingResults.length === 0) {
    return;
  }
  const existingResult = existingResults[existingResults.length - 1];

  const actions = existingResult.querySelector('.actions');
  if(!actions) {
    // stopped already
    return;
  }

  actions.remove();
}

// Request the DB adapter to fetch more results
function requestMoreResults(resultId, maxRows) {
  const classList = document.querySelector('.actions').classList;
  if(!classList.contains('inprogress')) {
    classList.add('inprogress');
    fetch(`api:fetchMoreResults/${resultId}`, {
      method: 'POST',
      body: JSON.stringify({maxRows}),
    });
  }
}

// Request no more results
function requestStopResults(resultId) {
  fetch(`api:stopResults/${resultId}`, {
    method: 'POST',
    body: "{}",
  });
}

// Make a result value into something that looks nice in the UI
function prettyValue(val) {
  const e = domElement;

  if(val === null) {
    return e('b', 'NULL');

  } else if(val === true) {
    return e('b', {'class': 'positive'}, 'true');

  } else if(val === false) {
    return e('b', {'class': 'negative'}, 'false');

  } else if(typeof val === 'object') {
    const showBigValue = () => {
      fetch(`api:showBigValue`, {
        method: 'POST',
        body: JSON.stringify({
          format: 'b64',
          data: val.data,
        }),
      });
      return false;
    }

    return e('a', { href: '#', 'class': 'binary', onclick: showBigValue }, `(${Math.ceil(val.data.length * 3 / 4)} bytes)`);

  } else if(typeof val === 'string') {
    if(val.length > 32) {
      const showBigValue = () => {
        fetch(`api:showBigValue`, {
          method: 'POST',
          body: JSON.stringify({
            format: 'text',
            data: val,
          }),
        });
        return false;
      }

      return e('a', { href: '#', onclick: showBigValue }, val.substring(0, 32));
    }

    return val;
  }

  return val.toString();
}

// Add a new results table
function addResultTable(resultId, results) {
  const e = domElement;

  const table = e('table',
    e('thead',
      e('tr', {'class': 'pk'},
        ...results.columns.map(col => e('th', { title: col.type }, col.name)),
      ),
    ),
    e('tbody',
      ...results.rows.map(row => e('tr',
        ...row.map(col => e('td', prettyValue(col)))
      ))
    )
  );

  let actions = null;
  if(results.more) {
    actions = e('div', {'class': 'actions'},
      e('button', {onclick: () => requestStopResults(resultId)}, 'Stop'),
      e('button', {onclick: () => requestMoreResults(resultId)}, 'Get more'),
      e('button', {onclick: () => requestMoreResults(resultId, 0)}, 'Get the rest'),
    );

  } else if(results.total === 0 && results.affected > 0) {
    actions = e('p', {'class': 'success'}, `${results.affected} rows affected`);
  }

  const resultEl = e('div', {'class': 'result'},
    table,
    e('p', { 'class': 'total success'}, `${results.total} rows fetched${results.more ? ' so far': ''}, ${results.affected} affected`),
    actions,
  );
  document.getElementById('content').appendChild(resultEl);
  resultEl.scrollIntoView();
}

// Add more data to the last results table
function continueResultsTable(resultId, results) {
  const existingResults = document.querySelectorAll('#content>.result');
  if(existingResults.length === 0) {
    console.error("Results cleared, can't continue table!");
    return;
  }
  const existingResult = existingResults[existingResults.length - 1];

  const e = domElement;

  const tbody = existingResult.querySelector('tbody');
  tbody.append(...results.rows.map(row => e('tr',
    ...row.map(col => e('td', prettyValue(col)))
  )));

  existingResult.querySelector('.total').textContent = `${results.total} rows fetched${results.more ? ' so far': ''}, ${results.affected} affected`;
  if(!results.more) {
    existingResult.querySelector('.actions').remove();
  } else {
    existingResult.querySelector('.actions').classList.remove('inprogress');
  }
}

function addErrorMessage(message) {
  const e = domElement;
  const msg = e('div', {'class': 'result'}, e('p', {'class': 'error'}, message));
  document.getElementById('content').appendChild(msg);
  msg.scrollIntoView();
}

