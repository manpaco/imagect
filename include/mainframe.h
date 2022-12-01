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
        void updatePreview(wxBitmap &bm);
        void allocateMem();
        void setBindings();
        void overlayPanels();
        void applyChanges(wxCommandEvent &);
        wxBitmap createBitmap(Magick::Image &img);
        void onCropChange(CropEvent &);

        CanvasPanel *canvas = nullptr;
        ToolsPanel *tools = nullptr;
        wxSplitterWindow *mainSplitter = nullptr;
        wxSplitterWindow *sideSplitter = nullptr;
        PreviewPanel *preview = nullptr;
        wxBoxSizer *mainSizer = nullptr;
        Magick::Image *sourceImg = nullptr;
        Magick::Image *lowResImg = nullptr;

};

#endif // MAINFRAME_H
