#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include "wx/filedlg.h"
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
