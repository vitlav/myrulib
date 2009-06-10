/////////////////////////////////////////////////////
// Generated By wxActiveRecordGenerator v 1.2.0-rc3
/////////////////////////////////////////////////////


#ifndef __BOOKS_
#define __BOOKS_

////@@begin gen include
#include "wxActiveRecord.h"
#include <wx/wx.h>
#include <wx/string.h>
#include <wx/datetime.h>
#include "Archives.h"
#include "Authors.h"

////@@end gen include

////@@begin custom include
////@@end custom include

////@@begin gen forward
class Books;
class BooksRow;
class BooksRowSet;

class Archives;
class ArchivesRow;
class ArchivesRowSet;
class Authors;
class AuthorsRow;
class AuthorsRowSet;

////@@end gen forward

////@@begin custom forward
////@@end custom forward

////@@begin gen arClass
class Books: public wxSqliteActiveRecord{
protected:
	BooksRow* RowFromResult(DatabaseResultSet* result);
public:
	Books();
	Books(const wxString& name,const wxString& server=wxEmptyString,const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxT("books"));
	Books(DatabaseLayer* database,const wxString& table=wxT("books"));
	bool Create(const wxString& name,const wxString& server=wxEmptyString,const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxT("books"));
	
	BooksRow* New();
	bool Delete(int key);

	
	BooksRow* Id(int key);

	BooksRow* Where(const wxString& whereClause);
	BooksRowSet* WhereSet(const wxString& whereClause,const wxString& orderBy=wxEmptyString);
	BooksRowSet* All(const wxString& orderBy=wxEmptyString); 

////@@begin custom arClass
public:
////@@end custom arClass
};
////@@end gen arClass

////@@begin gen arRow
class BooksRow: public wxActiveRecordRow{
public:
	BooksRow();
	BooksRow(const BooksRow& src);
	BooksRow(Books* activeRecord);
	BooksRow(DatabaseLayer* database,const wxString& table=wxT("books"));
	BooksRow& operator=(const BooksRow& src);
	bool GetFromResult(DatabaseResultSet* result);
public:
	wxString genres;
	wxString description;
	int id;
	int id_archive;
	int id_sequence;
	int file_size;
	wxString annotation;
	wxString file_name;
	wxString deleted;
	wxString title;
	int id_author;

public:
	ArchivesRow* GetArchive();
	AuthorsRowSet* GetAuthors(const wxString& orderBy=wxEmptyString);

	
	bool Save();
	bool Delete();
	
	
////@@begin custom arRow
public:
////@@end custom arRow	

};
////@@end gen arRow

////@@begin gen arSet
class BooksRowSet: public wxActiveRecordRowSet{
public:
	BooksRowSet();
	BooksRowSet(wxActiveRecord* activeRecord);
	BooksRowSet(DatabaseLayer* database,const wxString& table=wxT("books"));
	virtual BooksRow* Item(unsigned long item);
	
	virtual bool SaveAll();
	
	
protected:
	static int CMPFUNC_genres(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_description(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_id(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_id_archive(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_id_sequence(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_file_size(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_annotation(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_file_name(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_deleted(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_title(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_id_author(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_global(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	virtual CMPFUNC_proto GetCmpFunc(const wxString& var) const;

////@@begin custom arSet
public:
////@@end custom arSet
};
////@@end gen arSet

#endif /* __BOOKS_ */