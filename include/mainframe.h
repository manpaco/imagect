#ifndef MAINFRAME_H
#define MAINFRAME_H

class ToolsPanel;
class PreviewPanel;
class CropEvent;
class wxBoxSizer;
class wxButton;
class wxSplitterWindow;
class ScrolledView;
namespace Magick {
    class Image;
}
class ZoomCtrl;
class ZoomEvent;

#include "wx/frame.h"
#include "optscontainer.h"
#include <stack>

class MainFrame: public wxFrame {
    
    public:
        MainFrame();
        MainFrame(const wxString &initImg);
        ~MainFrame();

    private:
        void initParams();
        void allocateMem();
        void bindMenuBar();
        void bindElements();
        void unbindElements();
        void overlayPanels();
        void saveState(wxCommandEvent &);
        void composePreview();
        void onCropChange(CropEvent &);
        void updateHistory(OptionsContainer toSave);
        void redo(wxCommandEvent &);
        void undo(wxCommandEvent &);
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
        void onZoomChange(ZoomEvent &event);

        ScrolledView *sView = nullptr;
        ToolsPanel *tools = nullptr;
        wxSplitterWindow *mainSplitter = nullptr;
        wxSplitterWindow *sideSplitter = nullptr;
        PreviewPanel *preview = nullptr;
        wxBoxSizer *mainSizer = nullptr;
        Magick::Image *sourceImg = nullptr;
        Magick::Image *compImg = nullptr;

        wxButton *apply;
        wxButton *reset;
        ZoomCtrl *zoom;

        wxMenuBar *topMenuBar;
        wxMenu *mFile, *mEdit, *mHelp;

        std::stack<OptionsContainer> undoStack;
        OptionsContainer currentState;
        std::stack<OptionsContainer> redoStack;

        bool openedImg, exportedImg;

        int minMainSplitterSize, minSideSplitterSize;
        const int mainSplitterFactor = 4;
        const int sideSplitterFactor = 3;

};

#endif // MAINFRAME_H
