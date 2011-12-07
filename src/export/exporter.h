#ifndef EXPORTER_H
#define EXPORTER_H

#include <QHash>
#include <KIcon>

class QActionGroup;
class QObject;
class Column;

/**
 * \brief An iterator for table rows/columns.
 *
 * This is used to abstract how the results are stored in memory from
 * from the exporter.
 * 
 * After creation, the iterator points to an invalid row. You must call 
 * nextTable(), nextRow() and nextColumn() before value() returns anything meaningful.
 *
 */
class TableCellIterator {
public:
	TableCellIterator() { }
	virtual ~TableCellIterator() { }

	/**
	 * \brief Get the number of columns in the table.
	 * \return column count
	 */
	virtual int columns() const = 0;

	/**
	 * \brief Get the number of data rows in the table
	 *
	 * This may return -1 if the row count is unknown (e.g.
	 * when data is being streamed straight from the database and
	 * the database doesn't provide the row count beforehand)
	 *
	 * Note. The header row is not included in this.
	 * \return row count or -1 if unknown
	 */
	virtual int rows() const = 0;

	/**
	 * \brief Get column metadata
	 *
	 * Note. Not all column metadata may be available.
	 */
	virtual const Column& header(int column) const = 0;

	/**
	 * \brief Get the query string for the results
	 *
	 * This is the SQL query that was executed to produce
	 * these results. An empty string is returned if the
	 * query string is not available.
	 * \return query string
	 */
	virtual const QString& query() const = 0;

	/**
	 * \brief Move on to the next column
	 * \return true if column was available
	 */
	virtual bool nextColumn() = 0;

	/**
	 * \brief Move on to the next row.
	 *
	 * The column pointer is reset. You must call
	 * nextColumn() as well before value() returns anything
	 * meaningful.
	 * \return true if row was available
	 */
	virtual bool nextRow() = 0;

	/**
	 * \brief Move on to the next table (if any)
	 *
	 * The exporters do not call this function themselves.
	 * The row pointer is reset. You must call nextRow() and
	 * nextColumn() before value() returns anything meaningful.
	 *
	 * \return true if another table was available.
	 */
	virtual bool nextTable() = 0;

	/**
	 * \brief Get the value of the current table cell.
	 * \return cell value
	 */
	virtual QVariant value() const = 0;

};

/**
 \brief Abstract base class for result exporters
 */
class Exporter {
public:
	Exporter() { }
	virtual ~Exporter() { };

	/**
	 * \brief Start a new file
	 *
	 * \param file the file to write to
	 * \param encoding the requested encoding (for text files)
	 */
	virtual void startFile(QIODevice *file, const QString& encoding) = 0;

	/**
	 \brief Start a new table.

	 If this is not a multiTable exporter, this method will be called
	 only once.
	 */
	virtual void beginTable(TableCellIterator *iterator) = 0;

	/**
	 \brief Called once all tables have been iterated.
	 */
	virtual void done() = 0;
};

/**
 \brief An abstract base class for exporter factories.
 */
class ExporterFactory {
public:
	/**
	 \brief Construct an exporter factory.

	 An exporter is used to save the query results. Exporters can either
	 handle only a single result table at a time or multiple tables.

	 \param format the name of the format the exporter handles
	 \param ext the default file extension for this format
	 \param multitable can this exporter export more than one table at a time?
	 \param icon the icon used in menus for this format
	 */
	ExporterFactory(const QString& format, const QString& ext, bool multitable, const QString &icon=QString());
	virtual ~ExporterFactory() {}

	/**
	 \brief Get the format of this exporter
	 \return format name
	 */
	const QString& format() const { return m_format; }

	/**
	 \brief Get the icon for the export format
	 \return Icon
	 */
	KIcon icon() const { return KIcon(m_icon); }

	/**
	 \brief Get the default file extension for this format
	 \return file extension (leading . is not included)
	 */
	const QString& ext() const { return m_ext; }

	/**
	 \brief Can this exporter export multiple tables in the same
	 output file?

	 \return true if exporter can handle multiple tables
	 */
	bool isMultiTable() const { return m_multitable; }

	/**
	  \brief Return a new instance of the exporter

	  \return new exporter instance
	  */
	virtual Exporter *make() const = 0;

private:
	QString m_format;
	QString m_ext;
	bool m_multitable;
	QString m_icon;
};

/**
 \brief Available result exporters

 */
class Exporters {
public:
	/**
	 \brief Get the Exporters singleton instance.

	 \return exportes singleton
	 */
	static Exporters &instance();

	/**
	 \brief Register an exporter.

	 This is called automatically from ExporterFactory constructor.
	 
	 \param factory the factory to register
	 */
	void registerExporter(ExporterFactory *factory);

	/**
	 \brief Get an exporter for the given format
	 \param format the desired output format
	 \return new exporter instance or 0 if format not available
	 */
	Exporter *get(const QString& format) const;

	/**
	 * \brief Get the default file extension for the format
	 * \param format
	 * \return format
	 */
	QString getExtension(const QString& format) const;

	/**
	 * \brief Get all the registered exporter factories.
	 * \return list of exporter factories
	 */
	QList<ExporterFactory*> exporters() const { return m_factory.values(); }

	/**
	 \brief Get a list of actions for multitable exporters
 
	 This function returns a list of actions that can be plugged
	 into a menu.

	 The actions will be given the name "exportresult_X", where X is the export format. Also, the property "fileExtension" will be set to the default file extension.
	 \parent parent for the action group
	 \return group of new actions
	 */
	QActionGroup *multiTableActions(QObject *parent=0) const;

private:
	Exporters() {};
	QHash<QString,ExporterFactory*> m_factory;
};

#endif

