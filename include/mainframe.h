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

/**
 * Implementation of a main frame.
 *
 * Panels, buttons, controls and event handlers are used to offer an intuitive
 * main window.
 */
class MainFrame: public wxFrame {
    
    public:
        /**
         * Default Ctor.
         */
        MainFrame();

        /**
         * Construct a main frame to handle a given image file.
         */
        MainFrame(const wxString &initImg);

        /**
         * Dtor.
         */
        ~MainFrame();

    private:
        // ------------------------ EVENT HANDLERS ----------------------------
        void saveState(wxCommandEvent &);
        void redo(wxCommandEvent &);
        void undo(wxCommandEvent &);
        void onFixRatio(wxCommandEvent &);
        void onAllowGrow(wxCommandEvent &event);
        void onClose(wxCommandEvent &event);
        void onOpen(wxCommandEvent &event);
        void onExport(wxCommandEvent &event);
        void onQuit(wxCommandEvent &event);
        void onQuitFrame(wxCloseEvent &event);
        void onAbout(wxCommandEvent &event);
        void resetCrop(wxCommandEvent &event);
        void onZoomChange(ZoomEvent &event);
        void onCropChange(CropEvent &);
        // ---------------------- END EVENT HANDLERS --------------------------

        void initParams();
        void allocateMem();
        void bindMenuBar();
        void bindElements();
        void unbindElements();
        void overlayPanels();
        void createMenuBar();
        void clear();

        /**
         * Compose current state on the compressed image.
         */
        void composePreview();

        /**
         * Save the given container as current state and update stack.
         */
        void updateHistory(OptionsContainer toSave);

        /**
         * Show export dialog.
         */
        void exportImage(const wxString &p);

        /**
         * Show open dialog.
         */
        void openImage(const wxString &p);

        /**
         * Initialize frame dimensions based on the screen resolution.
         */
        void initDimensions();

        int showProceedMessage();
        int showCloseMessage();

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
