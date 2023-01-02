#ifndef MAINFRAME_H
#define MAINFRAME_H

class CanvasPanel;
class ToolsPanel;
class PreviewPanel;
class CropEvent;
class wxBoxSizer;
class wxButton;
class wxSplitterWindow;
namespace Magick {
    class Image;
}

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
        void updateCropGeometry(wxPoint &o, wxSize &s);
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

        std::stack<OptionsContainer> undoStack;
        OptionsContainer currentState;
        std::stack<OptionsContainer> redoStack;

        bool openedImg, exportedImg;

        int minMainSplitterSize, minSideSplitterSize;
        const int mainSplitterFactor = 4;
        const int sideSplitterFactor = 3;

};

#endif // MAINFRAME_H
