#include "queryresults.h"

QueryResults::QueryResults()
	: m_d(new QueryResultsData)
{
}

QueryResults::QueryResults(QueryResultsData *data)
	: m_d(data)
{
}

