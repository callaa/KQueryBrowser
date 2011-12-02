#include <QVariant>

#include "exporter.h"
#include "../meta/column.h"

class HtmlExporter : public Exporter {
public:
	HtmlExporter();

	void startFile(QIODevice *file);
	void beginTable(TableCellIterator *iterator);
	void done();

private:
	QIODevice *m_file;
};

class HtmlExporterFactory : public ExporterFactory
{
	public:
	HtmlExporterFactory()
		: ExporterFactory("HTML", "html", true, KIcon("text-html"))
	{
	}

	Exporter *make() const { return new HtmlExporter(); }
};

static HtmlExporterFactory htmlexporter;

HtmlExporter::HtmlExporter()
{
}

void HtmlExporter::startFile(QIODevice *file)
{
	m_file = file;
	m_file->write("<!DOCTYPE html>\n<html><body>\n");
}

static QByteArray esc(QString text) {
        return text.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;").toUtf8();
}

void HtmlExporter::beginTable(TableCellIterator *iterator)
{
	m_file->write("\n<table><thead>\n\t<tr>\n");
	for(int i=0;i<iterator->columns();++i) {
		m_file->write("\t\t<th>");
		m_file->write(esc(iterator->header(i).name()));
		m_file->write("</th>\n");
	}
	m_file->write("\t</tr>\n</thead><tbody>\n");
	while(iterator->nextRow()) {
		m_file->write("\t<tr>\n");
		while(iterator->nextColumn()) {
			m_file->write("\t\t<td>");
			m_file->write(esc(iterator->value().toString()));
			m_file->write("</td>\n");
		}
		m_file->write("\t</tr>\n");
	}
	m_file->write("</tbody></table>\n");
}

void HtmlExporter::done()
{
	m_file->write("\n</body></html>");
}

