#include "exporter.h"

#include <QActionGroup>
#include <QCoreApplication>
#include <QDebug>

Exporters &Exporters::instance()
{
	static Exporters e;
	return e;
}

void Exporters::registerExporter(ExporterFactory *factory)
{
	if(m_factory.contains(factory->format()))
		qWarning() << "Format" << factory->format() << "already registerd.";
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

QString Exporters::getExtension(const QString& format) const
{
	if(m_factory.contains(format))
		return m_factory.value(format)->ext();
	return QString();
}

QActionGroup *Exporters::multiTableActions(QObject *parent) const
{
	QActionGroup *grp = new QActionGroup(parent);
	foreach(ExporterFactory *f, m_factory.values()) {
		if(f->isMultiTable()) {
			QAction *a = new QAction(f->icon(), f->format(), grp);
			a->setObjectName("exportresult_" + f->format());
			a->setProperty("fileExtension", f->ext());
		}
	}
	return grp;
}

ExporterFactory::ExporterFactory(const QString& format, const QString &ext, bool multitable, const QString &icon)
	: m_format(format), m_ext(ext), m_multitable(multitable), m_icon(icon)
{
	Exporters::instance().registerExporter(this);
}
