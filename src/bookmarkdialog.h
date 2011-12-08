#ifndef BOOKMARKDIALOG_H
#define BOOKMARKDIALOG_H

#include <KBookmarkDialog>

/**
 * \brief A specialized bookmark dialog
 *
 * Currently, this just sets the bookmark icon according to the database type.
 */
class BookmarkDialog : public KBookmarkDialog {
	Q_OBJECT
public:
	BookmarkDialog(KBookmarkManager *mgr, QWidget *parent=0);

protected:
	void save(BookmarkDialogMode mode, const KBookmark &bm);
};

#endif

