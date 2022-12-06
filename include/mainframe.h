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

typedef std::tuple<wxPoint, OptionsContainer> State;

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
        Magick::Image composeState(const Magick::Image &img, const State &s);
        void composePreview();
        wxBitmap createBitmap(Magick::Image &img);
        void onCropChange(CropEvent &);
        void updateHistory(State toSave);
        void redo(wxCommandEvent &);
        void undo(wxCommandEvent &);
        void updateCropGeometry();
        void onFixRatio(wxCommandEvent &);
        void onAllowGrow(wxCommandEvent &event);
        void createMenuBar();
        void onClose(wxCommandEvent &event);
        void onOpen(wxCommandEvent &event);
        void onExport(wxCommandEvent &event);
        void onQuit(wxCommandEvent &event);
        void exportImage();
        void openImage(const wxString &p);
        void clear();
        void onQuitFrame(wxCloseEvent &event);
        int showProceedMessage();
        int showCloseMessage();

        CanvasPanel *canvas = nullptr;
        ToolsPanel *tools = nullptr;
        wxSplitterWindow *mainSplitter = nullptr;
        wxSplitterWindow *sideSplitter = nullptr;
        PreviewPanel *preview = nullptr;
        wxBoxSizer *mainSizer = nullptr;
        Magick::Image *sourceImg = nullptr;
        Magick::Image *scaledImg = nullptr;

        wxMenuBar *topMenuBar;
        wxMenu *mFile, *mEdit, *mHelp;

        std::list<State> history{};
        std::list<State>::iterator currentState;

        bool openedImg, exportedImg;

};

#endif // MAINFRAME_H
