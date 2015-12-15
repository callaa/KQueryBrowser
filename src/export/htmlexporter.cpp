#include "exporter.h"
#include "../meta/column.h"

#include <QVariant>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QStandardPaths>

class HtmlExporter : public Exporter {
public:
	HtmlExporter() : m_out(0), m_tplfile(0), m_tpl(0) { }
	~HtmlExporter() {
		delete m_tpl;
		delete m_tplfile;
		delete m_out;
	}

	void startFile(QIODevice *file, const QString& encoding, const QString& title);
	void beginTable(TableCellIterator *iterator);
	void done();

private:
	QTextStream *m_out;

	QFile *m_tplfile;
	QTextStream *m_tpl;
};

class HtmlExporterFactory : public ExporterFactory
{
	public:
	HtmlExporterFactory()
		: ExporterFactory("HTML", "html", true, "text-html")
	{
	}

	Exporter *make() const { return new HtmlExporter(); }
};

static HtmlExporterFactory htmlexporter;

static QString esc(QString text) {
        return text.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;");
}

void HtmlExporter::startFile(QIODevice *file, const QString& encoding, const QString& title)
{
	// Output stream in requested encoding
	m_out = new QTextStream(file);
	m_out->setCodec(QTextCodec::codecForName(encoding.toUtf8()));

	// Template input stream (template is in UTF-8)
	m_tplfile = new QFile(":ui/export.html");
	m_tplfile->open(QIODevice::ReadOnly | QIODevice::Text);
	m_tpl = new QTextStream(m_tplfile);

	while(true) {
		QString line = m_tpl->readLine();
		if(line.isNull() || line == "<!--MERGE-->")
				break;
		else if(line=="<!--TITLE-->")
			*m_out << esc(title);
		else if(line=="<!--CONTENT-TYPE-->")
			*m_out << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=" << encoding << "\">\n";
		else
			*m_out << line << '\n';
	}
}

void HtmlExporter::beginTable(TableCellIterator *iterator)
{
	*m_out << "<div class=\"query\">\n<p class=\"query\">";
	*m_out << esc(iterator->query());
	*m_out << "</p>\n<table><thead>\n\t<tr>\n";
	for(int i=0;i<iterator->columns();++i) {
		*m_out << "\t\t<th>";
		*m_out << esc(iterator->header(i).name());
		*m_out << "</th>\n";
	}
	*m_out << "\t</tr>\n</thead><tbody>\n";
	while(iterator->nextRow()) {
		*m_out << "\t<tr>\n";
		while(iterator->nextColumn()) {
			*m_out << "\t\t<td>";
			QVariant value = iterator->value();
			if(value.isNull())
				*m_out << "<b>NULL</b>";
			else
				*m_out << esc(value.toString());
			*m_out << "</td>\n";
		}
		*m_out << "\t</tr>\n";
	}
	*m_out << "</tbody></table>\n</div>\n";
}

void HtmlExporter::done()
{
	while(true) {
		QString line = m_tpl->readLine();
		if(line.isNull())
			break;
		*m_out << line << '\n';
	}

	delete m_tpl;
	delete m_tplfile;
	delete m_out;
	m_tpl = 0;
	m_tplfile = 0;
	m_out = 0;
}

