#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "Magick++/Image.h"
#include "composer.h"
#include <wx/wxprec.h>
#include "wx/splitter.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "canvaspanel.h"
#include "toolspanel.h"
#include "previewpanel.h"

class MainFrame: public wxFrame {
    
    public:
        MainFrame();

    private:
        void onExit(wxCloseEvent &event);
        void updatePreview();
        void allocateMem();
        void setBindings();
        void overlayPanels();
        void changeGrowState(wxCommandEvent &event);
        void changeGrowChoice(wxCommandEvent &event);

        CanvasPanel *canvas;
        ToolsPanel *tools;
        wxSplitterWindow *mainSplitter;
        wxSplitterWindow *sideSplitter;
        PreviewPanel *preview;
        wxBoxSizer *mainSizer;
        Magick::Image *imgTest;

};

#endif // MAINFRAME_H
