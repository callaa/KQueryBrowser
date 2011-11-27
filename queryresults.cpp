#include "queryresults.h"

QueryResultsData::QueryResultsData()
	: select(false), success(false), more(false), continuation(false),
	rowcount(-1)
{
}

QueryResults::QueryResults()
	: m_d(new QueryResultsData)
{
}

QueryResults::QueryResults(QueryResultsData *data)
	: m_d(data)
{
}

