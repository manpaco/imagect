#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "Magick++/Image.h"
#include <wx/wxprec.h>
#include "wx/splitter.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "canvaspanel.h"
#include "toolspanel.h"
#include "previewpanel.h"
#include "cropevent.h"

class MainFrame: public wxFrame {
    
    public:
        MainFrame();
        ~MainFrame();

    private:
        void updatePreview();
        void allocateMem();
        void setBindings();
        void overlayPanels();
        void applyChanges(wxCommandEvent &);
        wxBitmap * createBitmap(Magick::Image *img);
        void onCropChange(CropEvent &);

        CanvasPanel *canvas;
        ToolsPanel *tools;
        wxSplitterWindow *mainSplitter;
        wxSplitterWindow *sideSplitter;
        PreviewPanel *preview;
        wxBoxSizer *mainSizer;
        Magick::Image *highResImg;
        Magick::Image *lowResImg;
        wxBitmap *lowResBitmap;

};

#endif // MAINFRAME_H
