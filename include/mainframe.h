#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#if wxUSE_STATLINE
    #include <wx/statline.h>
#endif

#if wxUSE_SPLITTER
    #include <wx/splitter.h>
#endif

#include "Magick++/Image.h"
#include "canvaspanel.h"
#include "toolspanel.h"
#include "previewpanel.h"
#include "cropevent.h"

class MainFrame: public wxFrame {
    
    public:
        MainFrame();
        MainFrame(const wxString &initImg);
        ~MainFrame();

    private:
        void initParams();
        void updatePreview(wxBitmap &bm);
        void allocateMem();
        void bindMenuBar();
        void bindElements();
        void unbindElements();
        void overlayPanels();
        void saveState(wxCommandEvent &);
        void composePreview();
        void onCropChange(CropEvent &);
        void updateHistory(State toSave);
        void redo(wxCommandEvent &);
        void undo(wxCommandEvent &);
        void updateCropGeometry(State &s);
        void onFixRatio(wxCommandEvent &);
        void onAllowGrow(wxCommandEvent &event);
        void createMenuBar();
        void onClose(wxCommandEvent &event);
        void onOpen(wxCommandEvent &event);
        void onExport(wxCommandEvent &event);
        void onQuit(wxCommandEvent &event);
        void exportImage(const wxString &p);
        void openImage(const wxString &p);
        void clear();
        void onQuitFrame(wxCloseEvent &event);
        int showProceedMessage();
        int showCloseMessage();
        void onAbout(wxCommandEvent &event);
        void initDimensions();
        void resetCrop(wxCommandEvent &event);

        CanvasPanel *canvas = nullptr;
        ToolsPanel *tools = nullptr;
        wxSplitterWindow *mainSplitter = nullptr;
        wxSplitterWindow *sideSplitter = nullptr;
        PreviewPanel *preview = nullptr;
        wxBoxSizer *mainSizer = nullptr;
        Magick::Image *sourceImg = nullptr;
        Magick::Image *scaledImg = nullptr;

        wxButton *apply;
        wxButton *reset;

        wxMenuBar *topMenuBar;
        wxMenu *mFile, *mEdit, *mHelp;

        std::list<State> history{};
        std::list<State>::iterator currentState;

        bool openedImg, exportedImg;

        int minMainSplitterSize, minSideSplitterSize;
        const int mainSplitterFactor = 4;
        const int sideSplitterFactor = 3;

};

#endif // MAINFRAME_H
