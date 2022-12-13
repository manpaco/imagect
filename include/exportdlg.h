#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "defs.h"

class ExportDialog: public wxFileDialog {
    public:
        ExportDialog(wxWindow *parent);
        wxString validPath();
        ~ExportDialog();

    private:
        void initWildcards();

        std::string imageWcs[ict::WILDCARD_SIZE];
        std::string exportWc;
};

#endif // EXPORTDLG_H
