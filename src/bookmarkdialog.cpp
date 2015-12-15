#include "bookmarkdialog.h"

BookmarkDialog::BookmarkDialog(KBookmarkManager *mgr, QWidget *parent)
	: KBookmarkDialog(mgr, parent)
{
}

#if 0
void BookmarkDialog::save(BookmarkDialogMode mode, const KBookmark &bm)
{
	Q_UNUSED(bm);
	switch(mode) {
		case KBookmarkDialog::NewBookmark:
		case KBookmarkDialog::EditBookmark :
			m_bm.setIcon(":/icons/" + m_bm.url().scheme() + ".png");
		default: break;
	}
}
#endif
