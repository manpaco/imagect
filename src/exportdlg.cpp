#include "exportdlg.h"
#include "defs.h"
#include "filext.h"

ExportDialog::ExportDialog(wxWindow *parent): 
    wxFileDialog(parent, _("Export image"),wxEmptyString, wxEmptyString, wxEmptyString, 
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT) {
    initWildcards();
    SetWildcard(exportWc);
}

wxString ExportDialog::validPath() {
    wxString fn = GetFilename();
    wxString p = GetPath();
    if(!extension(fn.ToStdString())) {
        p = GetDirectory();
        switch(GetFilterIndex()) {
        case ict::PNG_WC:
            fn.Append(pngExt);
            break;
        case ict::JPEG_WC:
            fn.Append(jpgExt);
            break;
        }
        p.Append(fn);
    }
    return p;
}

void ExportDialog::initWildcards(){
    imageWcs[ict::PNG_WC] = pngWc;
    imageWcs[ict::JPEG_WC] = jpegWc;
    exportWc = imageWcs[0];
    for(int i = 1; i < ict::WILDCARD_SIZE; i++) {
        exportWc += "|" + imageWcs[i];
    }
}

ExportDialog::~ExportDialog() {

}
