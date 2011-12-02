#include <QVariant>
#include <QFile>

#include <KStandardDirs>

#include "exporter.h"
#include "../meta/column.h"

class HtmlExporter : public Exporter {
public:
	HtmlExporter();
	~HtmlExporter() { delete m_tpl; }

	void startFile(QIODevice *file);
	void beginTable(TableCellIterator *iterator);
	void done();

private:
	QIODevice *m_file;
	QFile *m_tpl;
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
	m_tpl = new QFile(KStandardDirs::locate("appdata", "export.html"));
	m_tpl->open(QIODevice::ReadOnly | QIODevice::Text);
	while(true) {
		QByteArray line = m_tpl->readLine();
		if(line.isEmpty() || line.startsWith("--MERGE--"))
				break;
		m_file->write(line);
	}
}

static QByteArray esc(QString text) {
        return text.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;").toUtf8();
}

void HtmlExporter::beginTable(TableCellIterator *iterator)
{
	m_file->write("<div class=\"query\">\n<p class=\"query\">");
	m_file->write(esc(iterator->query()));
	m_file->write("</p>\n<table><thead>\n\t<tr>\n");
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
			QVariant value = iterator->value();
			if(value.isNull())
				m_file->write("<b>NULL</b>");
			else
				m_file->write(esc(value.toString()));
			m_file->write("</td>\n");
		}
		m_file->write("\t</tr>\n");
	}
	m_file->write("</tbody></table>\n</div>\n");
}

void HtmlExporter::done()
{
	char buffer[1024];
	int len;
	while((len=m_tpl->read(buffer, sizeof buffer))>0)
			m_file->write(buffer, len);
	delete m_tpl;
	m_tpl = 0;
}

