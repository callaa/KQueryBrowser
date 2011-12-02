#include <QActionGroup>
#include <QCoreApplication>

#include "exporter.h"

Exporters &Exporters::instance()
{
	static Exporters e;
	return e;
}

void Exporters::registerExporter(ExporterFactory *factory)
{
	if(m_factory.contains(factory->format()))
		qWarning("Format %s already registerd.", factory->format().toAscii().constData());
	m_factory.insert(factory->format(), factory);
}

Exporter *Exporters::get(const QString& format) const
{
	if(m_factory.contains(format)) {
		ExporterFactory *f = m_factory.value(format);
		return f->make();
	}
	return 0;
}

QActionGroup *Exporters::multiTableActions(QObject *parent) const
{
	QActionGroup *grp = new QActionGroup(parent);
	foreach(ExporterFactory *f, m_factory.values()) {
		if(f->isMultiTable()) {
			QAction *a = new QAction(f->icon(), QCoreApplication::tr("Export %1").arg(f->format()), grp);
			a->setObjectName("exportresult_" + f->format());
			a->setProperty("fileExtension", f->ext());
		}
	}
	return grp;
}

ExporterFactory::ExporterFactory(const QString& format, const QString &ext, bool multitable, const KIcon &icon)
	: m_format(format), m_ext(ext), m_multitable(multitable), m_icon(icon)
{
	Exporters::instance().registerExporter(this);
}