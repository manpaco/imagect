#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include "wx/filedlg.h"
#include "defs.hpp"

/**
 * Implementation of a export dialog.
 *
 * Generic wxFileDialog with custom wildcards and path validation.
 */
class ExportDialog: public wxFileDialog {
    public:
        /**
         * Ctor.
         */
        ExportDialog(wxWindow *parent);

        /**
         * Return the path selected by the user with valid extension.
         */
        wxString validPath();

        /**
         * Dtor.
         */
        ~ExportDialog();

    private:
        /**
         * Initialize wildcards
         */
        void initWildcards();

        std::string imageWcs[ict::WILDCARD_SIZE];
        std::string exportWc;
};

#endif // EXPORTDLG_H
