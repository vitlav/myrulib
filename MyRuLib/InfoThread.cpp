#include "InfoThread.h"
#include "ZipReader.h"
#include "InfoCash.h"
#include "ParseCtx.h"

void *InfoThread::Entry()
{
    ZipReader reader(m_id);
    if (!reader.IsOK()) return NULL;

	Load(reader.GetZip());

	return NULL;
}

void InfoThread::Execute(wxEvtHandler *frame, const int id)
{
    if (!id) return;
	InfoThread *thread = new InfoThread(frame, id);
    if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
}

class InfoParsingContext: public ParsingContext
{
public:
    wxString annotation;
    wxString imagedata;
    wxString imagetype;
    wxString imagename;
    bool skipimage;
	wxArrayString images;
};

extern "C" {
static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;

    wxString node_name = ParsingContext::CharToLower(name);

	if (ctx->Path(4) == wxT("fictionbook/description/title-info/annotation/")) {
		ctx->annotation += wxString::Format(wxT("<%s"), node_name.c_str());
		const XML_Char **a = atts;
		while (*a) {
            wxString name = ParsingContext::CharToLower(a[0]);
            wxString value = ParsingContext::CharToLower(a[1]);
			ctx->annotation += wxString::Format(wxT(" %s=%s"), name.c_str(), value.c_str());
			a += 2;
		}
		ctx->annotation += wxT(">");
	} else {
		wxString path = ctx->Path();
		if (path == wxT("fictionbook/description/title-info/coverpage/")) {
			if (node_name == wxT("image")) {
				const XML_Char **a = atts;
				while (*a) {
					wxString name = ParsingContext::CharToLower(a[0]);
                    wxString value = ParsingContext::CharToLower(a[1]);
					if (name == wxT("l:href")) {
						if (value.Left(1) == wxT("#")) value = value.Mid(1);
                        wxString imagename = wxString::Format(wxT("%d/%s"), ctx->m_id, value.c_str());
                        ctx->images.Add(value);
					}
					a += 2;
				}
			}
        } else if ((node_name == wxT("binary")) && path == wxT("fictionbook/")) {
            ctx->skipimage = true;
            ctx->imagedata.Empty();
            ctx->imagetype.Empty();
            ctx->imagename.Empty();
            const XML_Char **a = atts;
            while (*a) {
                wxString name = ParsingContext::CharToLower(a[0]);
                wxString value = ParsingContext::CharToLower(a[1]);
                if (name == wxT("id")) {
                    ctx->skipimage = (ctx->images.Index(value) == wxNOT_FOUND);
                    ctx->imagename = value;
                } else if (name == wxT("content-type")) {
                    ctx->imagetype = value;
                }
                a += 2;
            }
        }
	}
	ctx->AppendTag(node_name);
}
}

extern "C" {
static void EndElementHnd(void *userData, const XML_Char* name)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;
    wxString node_name = ParsingContext::CharToLower(name);

	if (ctx->Level()>4 && ctx->Path(4) == wxT("fictionbook/description/title-info/annotation/")) {
		ctx->annotation.Trim(false).Trim(true);
		ctx->annotation += wxString::Format(wxT("</%s>"), node_name.c_str());
	} else {
		wxString path = ctx->Path();
		if (path == wxT("fictionbook/description/title-info/")) {
	        InfoCash::SetAnnotation(ctx->m_id, ctx->annotation);
			if (!ctx->images.Count()) XML_StopParser(ctx->parser, XML_FALSE);
			wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOKINFO_UPDATE );
			event.SetInt(ctx->m_id);
			wxPostEvent( ctx->m_frame, event );
			ctx->annotation.Empty();
		} else if (path == wxT("fictionbook/binary/")) {
		    if (!ctx->skipimage) {
		        InfoCash::AddImage(ctx->m_id, ctx->imagename, ctx->imagedata, ctx->imagetype);
                wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOKINFO_UPDATE );
                event.SetInt(ctx->m_id);
                wxPostEvent( ctx->m_frame, event );
                ctx->annotation.Empty();
		    }
		}
	}
	ctx->RemoveTag(node_name);
}
}

extern "C" {
static void TextHnd(void *userData, const XML_Char *s, int len)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;

	if (ctx->Path(4) == wxT("fictionbook/description/title-info/annotation/")) {
	    wxString str = ParsingContext::CharToString(s, len);
	    if (!ParsingContext::IsWhiteOnly(str)) ctx->annotation += str;
	} else if (ctx->Path() == wxT("fictionbook/binary/")) {
	    wxString str = ParsingContext::CharToString(s, len);
	    if (!ParsingContext::IsWhiteOnly(str)) ctx->imagedata += str;
	}
}
}

extern "C" {
static void DefaultHnd(void *userData, const XML_Char *s, int len)
{
    // XML header:
    if (len > 6 && memcmp(s, "<?xml ", 6) == 0)
    {
        InfoParsingContext *ctx = (InfoParsingContext*)userData;

        wxString buf = ParsingContext::CharToString(s, (size_t)len);
        int pos;
        pos = buf.Find(wxT("encoding="));
        if (pos != wxNOT_FOUND)
            ctx->encoding = buf.Mid(pos + 10).BeforeFirst(buf[(size_t)pos+9]);
        pos = buf.Find(wxT("version="));
        if (pos != wxNOT_FOUND)
            ctx->version = buf.Mid(pos + 9).BeforeFirst(buf[(size_t)pos+8]);
    }
}
}

extern "C" {
static int UnknownEncodingHnd(void * WXUNUSED(encodingHandlerData),
                              const XML_Char *name, XML_Encoding *info)
{
    // We must build conversion table for expat. The easiest way to do so
    // is to let wxCSConv convert as string containing all characters to
    // wide character representation:
    wxString str(name, wxConvLibc);
    wxCSConv conv(str);
    char mbBuf[2];
    wchar_t wcBuf[10];
    size_t i;

    mbBuf[1] = 0;
    info->map[0] = 0;
    for (i = 0; i < 255; i++)
    {
        mbBuf[0] = (char)(i+1);
        if (conv.MB2WC(wcBuf, mbBuf, 2) == (size_t)-1)
        {
            // invalid/undefined byte in the encoding:
            info->map[i+1] = -1;
        }
        info->map[i+1] = (int)wcBuf[0];
    }

    info->data = NULL;
    info->convert = NULL;
    info->release = NULL;

    return 1;
}
}

bool InfoThread::Load(wxInputStream& stream)
{
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    InfoParsingContext ctx;
    bool done;
    XML_Parser parser = XML_ParserCreate(NULL);

	ctx.parser = parser;
	ctx.m_frame = m_frame;
	ctx.m_id = m_id;

    XML_SetUserData(parser, (void*)&ctx);
    XML_SetElementHandler(parser, StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(parser, TextHnd);
    XML_SetDefaultHandler(parser, DefaultHnd);
    XML_SetUnknownEncodingHandler(parser, UnknownEncodingHnd, NULL);

    bool ok = true;
    do {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);

        if ( !XML_Parse(parser, buf, len, done) ) {
			XML_Error error_code = XML_GetErrorCode(parser);
			if ( error_code == XML_ERROR_ABORTED ) {
				done = true;
			} else {
				wxString error(XML_ErrorString(error_code), *wxConvCurrent);
				wxLogError(_("XML parsing error: '%s' at line %d"), error.c_str(), XML_GetCurrentLineNumber(parser));
				ok = false;
	            break;
			}
        }
    } while (!done);

    XML_ParserFree(parser);

    return ok;
}
