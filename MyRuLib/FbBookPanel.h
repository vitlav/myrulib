#ifndef __FBBOOKPANEL_H__
#define __FBBOOKPANEL_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbBookList.h"
#include "FbBookData.h"
#include "FbParams.h"

enum FbListMode {
	FB2_MODE_LIST = 0,
	FB2_MODE_TREE = 1,
};

enum FbViewMode {
	FB2_VIEW_HORISONTAL = 0,
	FB2_VIEW_VERTICAL = 1,
	FB2_VIEW_NOTHING = 2,
};

class FbBookPanel: public wxSplitterWindow
{
	public:
		FbBookPanel(wxWindow *parent, const wxSize& size, long style, int keyType, int keyMode);
		void UpdateInfo(int id);
		void SetOrderID(int id);
		int GetOrderID();
		bool IsOrderDesc();
		void RevertOrder();
		wxString GetOrderSQL();
	public:
		FbBookList * m_BookList;
		FbItemData * GetSelectedBook();
		void EmptyBooks(const int selected  = 0);
		void AppendBook(BookTreeItemData & data, const wxString & authors = wxEmptyString);
		void AppendAuthor(int id, const wxString title, wxTreeItemData * data = NULL);
		void AppendSequence(int id, const wxString title, wxTreeItemData * data = NULL);
		void CreateColumns(FbListMode mode);
		FbListMode GetListMode() { return m_ListMode;};
		void SetFolder(int folder) { m_folder = folder; };
		void SetType(int type) { m_type = type; };
		void UpdateFonts(bool refresh = true);
	private:
		wxHtmlWindow * m_BookInfo;
		void SetViewMode(int mode);
		void DoFolderAdd(const int folder);
		static void DoDeleteDownload(const wxString &sel, const int folder);
		static void DoCreateDownload(const wxString &sel, int count = 1);
		int GetRatingColumn();
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		wxString F(const int number);
		wxString m_AuthorName;
		FbListMode m_ListMode;
		wxTreeItemId m_AuthorItem;
		wxTreeItemId m_SequenceItem;
		int m_folder;
		int m_type;
		int m_selected;
		int m_KeyView;
	private:
		void OnAuthorInfo(wxCommandEvent& event);
		void OnBooksListViewSelected(wxTreeEvent & event);
		void OnBooksListActivated(wxTreeEvent & event);
		void OnDeleteDownload(wxCommandEvent & event);
		void OnDownloadBook(wxCommandEvent & event);
		void OnEditComments(wxCommandEvent & event);
		void OnFavoritesAdd(wxCommandEvent & event);
		void OnFolderAdd(wxCommandEvent& event);
		void OnOpenAuthor(wxCommandEvent& event);
		void OnOpenBook(wxCommandEvent & event);
		void OnChangeView(wxCommandEvent & event);
		void OnBookPage(wxCommandEvent & event);
		void OnSystemDownload(wxCommandEvent & event);
		void OnInfoUpdate(wxCommandEvent& event);
		void OnSubmenu(wxCommandEvent& event);
		void OnContextMenu(wxTreeEvent& event);
		void OnSelectAll(wxCommandEvent& event);
		void OnUnselectAll(wxCommandEvent& event);
		void OnChangeRating(wxCommandEvent& event);
		void OnDeleteBooks(wxCommandEvent& event);
		void OnModifyBooks(wxCommandEvent& event);
		void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
		void OnChangeModeUpdateUI(wxUpdateUIEvent & event);
		DECLARE_EVENT_TABLE();
	private:
};


#endif // __FBBOOKPANEL_H__
