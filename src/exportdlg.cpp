#include "exportdlg.hpp"
#include "defs.hpp"
#include "filext.hpp"

ExportDialog::ExportDialog(wxWindow *parent):
    wxFileDialog(parent, _("Export image"), wxEmptyString, wxEmptyString,
                 wxEmptyString, wxFD_SAVE | wxFD_OVERWRITE_PROMPT) {
    initWildcards();
    SetWildcard(exportWc);
}

wxString ExportDialog::validPath() {
    wxString fn = GetFilename();
    wxString p = GetPath();
    if(!extension(fn.ToStdString())) {
        switch(GetFilterIndex()) {
        case ict::PNG_WC:
            p.Append(pngExt);
            break;
        case ict::JPEG_WC:
            p.Append(jpgExt);
            break;
        }
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
