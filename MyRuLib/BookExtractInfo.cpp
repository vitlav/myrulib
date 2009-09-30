#include "BookExtractInfo.h"

WX_DEFINE_OBJARRAY(BookExtractArrayBase);

BookExtractInfo::BookExtractInfo(wxSQLite3ResultSet & result):
	id_book(result.GetInt(wxT("id"))),
	id_archive(result.GetInt(wxT("id_archive"))),
    book_name(result.GetString(wxT("file_name"))),
    book_path(result.GetString(wxT("file_path"))),
    librusec(false)
{
    librusec = (id_book>0 && result.GetInt(wxT("file")) == 0);
}

wxString BookExtractInfo::GetBook()
{
    wxString result = book_path;
    if (!result.IsEmpty()) result += wxFileName::GetPathSeparator();
    result += book_name;
    return result;
}

wxString BookExtractInfo::GetZip(const wxString &path)
{
    wxString result = path.IsEmpty() ? zip_path : path;
    if (!result.IsEmpty()) result += wxFileName::GetPathSeparator();
    result += zip_name;
    return result;
}

bool BookExtractInfo::NameIsEqual()
{
    wxFileName filename = zip_name;
    return filename.GetName() == book_name;
}

BookExtractArray::BookExtractArray(FbDatabase & database, const int id)
    :BookExtractArrayBase(), m_id(id)
{
    {
        wxString sql = wxT("\
            SELECT DISTINCT 0 AS file, id, id_archive, file_name, file_path FROM books WHERE id=? UNION ALL \
            SELECT DISTINCT 1 AS file, id_book, id_archive, file_name, file_path FROM files WHERE id_book=? \
            ORDER BY file \
        ");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, id);
        stmt.Bind(2, id);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        while ( result.NextRow() ) Add(result);
    }

    {
        wxString sql = wxT("SELECT file_name, file_path FROM archives WHERE id=?");
        for (size_t i = 0; i<Count(); i++) {
            BookExtractInfo & item = Item(i);
            if (!item.id_archive) continue;
            wxSQLite3Statement stmt = database.PrepareStatement(sql);
            stmt.Bind(1, item.id_archive);
            wxSQLite3ResultSet result = stmt.ExecuteQuery();
            if (result.NextRow()) {
                item.zip_name = result.GetString(wxT("file_name"));
                item.zip_path = result.GetString(wxT("file_path"));
            }
        }
    }
}