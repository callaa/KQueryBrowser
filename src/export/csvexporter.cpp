#include <QVariant>
#include <QTextCodec>
#include <QTextStream>

#include "exporter.h"
#include "../meta/column.h"
#include "../stringbuilder.h"

class CsvExporter : public Exporter {
public:
	CsvExporter() : m_out(0) { }
	~CsvExporter() { delete m_out; }

	void startFile(QIODevice *file, const QString& encoding, const QString& title);
	void beginTable(TableCellIterator *iterator);
	void done();

private:
	void printRow(const QStringList& columns);
	QTextStream *m_out;
};

class CsvExporterFactory : public ExporterFactory
{
public:
	CsvExporterFactory()
	: ExporterFactory("CSV", "csv", false, "text-csv")
	{
	}

	Exporter *make() const { return new CsvExporter(); }
};

static CsvExporterFactory factory;

void CsvExporter::startFile(QIODevice *file, const QString& encoding, const QString& title)
{
	Q_UNUSED(title);
	// Output stream in requested encoding
	m_out = new QTextStream(file);
	m_out->setCodec(QTextCodec::codecForName(encoding.toUtf8()));
}

void CsvExporter::printRow(const QStringList &columns)
{
	bool first=true;
	foreach(const QString& c, columns) {
		// Comma separator
		if(first)
			first=false;
		else
			*m_out << ',';

		// Quote column if needed
		if(c.contains(',') || c.contains('"') || c.contains('\n')) {
			StringBuilder sb;
			sb << QString("\"");

			// Escape quote characters
			int quote=-1,lastquote=0;
			while((quote=c.indexOf('"', quote+1))>=0) {
				sb << c.mid(lastquote, quote-lastquote) << "\"";
				lastquote = quote;
			}
			sb << c.mid(lastquote);
			sb << QString("\"");

			*m_out << sb.toString();
		} else {
			*m_out << c;
		}
	}
		*m_out << "\r\n";
}

void CsvExporter::beginTable(TableCellIterator *iterator)
{
	// See RFC 4180 for format specification
	
	// Write header row
	QStringList cols;
	for(int i=0;i<iterator->columns();++i) {
		cols << iterator->header(i).name();
	}

	printRow(cols);

	// Write data rows
	while(iterator->nextRow()) {
		cols.clear();
		while(iterator->nextColumn())
			cols << iterator->value().toString();
		printRow(cols);
	}
}

void CsvExporter::done()
{
	delete m_out;
	m_out = 0;
}

