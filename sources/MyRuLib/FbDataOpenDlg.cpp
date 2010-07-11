#include "FbDataOpenDlg.h"
#include <wx/artprov.h>
#include <wx/wxsqlite3.h>
#include "FbLogoBitmap.h"
#include "FbDatabase.h"
#include "FbParams.h"
#include "FbConst.h"
#include "FbProgressDlg.h"
#include "FbScanerThread.h"

BEGIN_EVENT_TABLE( FbDataOpenDlg, FbDialog )
	EVT_COMBOBOX( ID_FILE_TXT, FbDataOpenDlg::OnFileCombo )
	EVT_BUTTON( ID_FILE_BTN, FbDataOpenDlg::OnSelectFileClick )
	EVT_BUTTON( ID_FOLDER_BTN, FbDataOpenDlg::OnSelectFolderClick )
END_EVENT_TABLE()

FbDataOpenDlg::FbDataOpenDlg( wxWindow* parent )
	: FbDialog( parent, wxID_ANY, GetTitle(), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
	wxStaticText * info;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer * bSizerTop = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBitmap * m_bitmap = new wxStaticBitmap( this, wxID_ANY, FbLogoBitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTop->Add( m_bitmap, 0, wxALL|wxALIGN_TOP, 10 );

	wxBoxSizer* bSizerCtrl;
	bSizerCtrl = new wxBoxSizer( wxVERTICAL );

	info = new wxStaticText( this, wxID_ANY, _("File name:"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	bSizerCtrl->Add( info, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5 );

	wxBoxSizer* bSizerFile;
	bSizerFile = new wxBoxSizer( wxHORIZONTAL );

	m_file.Create( this, ID_FILE_TXT, wxEmptyString, wxDefaultPosition, wxSize( 300,-1 ), 0, NULL, 0 );
	bSizerFile->Add( &m_file, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );
	for (size_t i = 0; i<=5; i++) {
		wxString filename = FbParams::GetText(i + FB_RECENT_0);
		if (!filename.IsEmpty()) m_file.Append(filename);
	}

	wxBitmapButton * btnFile = new wxBitmapButton( this, ID_FILE_BTN, wxArtProvider::GetBitmap(wxART_FILE_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizerFile->Add( btnFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerCtrl->Add( bSizerFile, 1, wxEXPAND, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Library folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	bSizerCtrl->Add( info, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer * bSizerFolder = new wxBoxSizer( wxHORIZONTAL );

	m_folder.Create( this, ID_FOLDER_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerFolder->Add( &m_folder, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );

	wxBitmapButton * btnFolder = new wxBitmapButton( this, ID_FOLDER_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizerFolder->Add( btnFolder, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	bSizerCtrl->Add( bSizerFolder, 1, wxEXPAND, 5 );

	bSizerTop->Add( bSizerCtrl, 1, wxEXPAND, 5 );
	bSizerMain->Add( bSizerTop, 0, wxEXPAND, 5 );

	m_scaner.Create( this, wxID_ANY, _("Update zip files catalog") );
	m_scaner.SetValue(true);
	bSizerCtrl->Add( &m_scaner, 0, wxALL|wxEXPAND, 5 );
/*
	m_import.Create( this, wxID_ANY, _("Run a full scan for library files") );
	m_import.SetValue(false);
	bSizerCtrl->Add( &m_import, 0, wxALL|wxEXPAND, 5 );
*/
	m_only.Create( this, wxID_ANY, _("Process only new files") );
	m_only.SetValue(true);
	bSizerCtrl->Add( &m_only, 0, wxALL|wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	m_file.SetFocus();
}

wxString FbDataOpenDlg::GetTitle() const
{
	return strProgramName + wxT(" - ") + _("Open (or create) collection");
}

void FbDataOpenDlg::OnSelectFileClick( wxCommandEvent& event )
{
	wxFileDialog dlg (
		this,
		_("Enter collection filename"),
		wxEmptyString,
		m_file.GetValue(),
		_("Database file (*.db)|*.db|All files (*.*)|*.*"),
		wxFD_OPEN,
		wxDefaultPosition
	);
	if (dlg.ShowModal() == wxID_OK) {
		m_file.SetValue(CheckExt(dlg.GetPath()));
		UpdateFolder();
	}
}

void FbDataOpenDlg::OnFileCombo( wxCommandEvent& event )
{
	UpdateFolder();
}

void FbDataOpenDlg::OnSelectFolderClick( wxCommandEvent& event )
{
	wxDirDialog dlg(
		this,
		_("Select folder"),
		m_folder.GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);
	if (dlg.ShowModal() == wxID_OK) {
		m_folder.SetValue(dlg.GetPath());
	}
}

void FbDataOpenDlg::UpdateFolder()
{
	wxFileName filename = m_file.GetValue();
	if (!filename.FileExists()) {
		m_folder.Clear();
		return;
	}

	wxSQLite3Database database;
	database.Open(filename.GetFullPath(), wxEmptyString, WXSQLITE_OPEN_READWRITE);
	if (!database.IsOpen()) {
		m_folder.Clear();
		return;
	}

	wxString text;
	wxString sql = wxT("SELECT text FROM params WHERE id="); sql << DB_LIBRARY_DIR;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (result.NextRow()) text = result.GetString(0);

	if (text.IsEmpty()) {
		m_folder.SetValue(filename.GetPath());
	} else {
		wxFileName path = text;
		if (!path.IsAbsolute()) path.MakeAbsolute(filename.GetPath());
		m_folder.SetValue(path.GetFullPath());
	}
}

wxString FbDataOpenDlg::GetFilename()
{
	return CheckExt(m_file.GetValue());
}

wxString FbDataOpenDlg::GetDirname()
{
	wxString dirname = m_folder.GetValue();
	if (dirname.IsEmpty()) dirname = wxFileName(m_file.GetValue()).GetPath();
	return dirname;
}

wxString FbDataOpenDlg::CheckExt(const wxString &filename)
{
	wxString result = filename;
	const wxChar * ext = wxT(".db");
	#ifdef __WXMSW__
	if (filename.Right(3).Lower() != ext) result += ext;
	#else
	if (filename.Right(3) != ext) result += ext;
	#endif
	return result;
}

bool FbDataOpenDlg::Execute(wxWindow * parent, wxString & filename)
{
	FbDataOpenDlg dlg(parent);
	if (dlg.ShowModal() != wxID_OK) return false;

	filename = dlg.GetFilename();

	if (dlg.m_scaner.GetValue()) {
		FbProgressDlg scaner(dlg.GetParent());
		FbThread * thread = new FbScanerThread(&scaner, filename, dlg.GetDirname(), dlg.m_only.GetValue());
		scaner.RunThread(thread);
		return scaner.ShowModal() == wxID_OK;
	}
	return true;
}
