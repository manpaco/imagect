#include "mainframe.hpp"
#include "cropevent.hpp"
#include "defs.hpp"
#include "buildvalues.hpp"
#include "previewpanel.hpp"
#include "toolspanel.hpp"
#include "filext.hpp"
#include "exportdlg.hpp"
#include "wx/settings.h"
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include <iostream>
#include "extendedcanvas.hpp"
#include "canvasitem.hpp"

#if wxUSE_STATLINE
    #include <wx/statline.h>
#endif

#if wxUSE_SPLITTER
    #include <wx/splitter.h>
#endif

MainFrame::MainFrame():
    wxFrame(NULL,
            wxID_ANY,
            projectName,
            wxDefaultPosition,
            wxDefaultSize,
            wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU |
            wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxMAXIMIZE) {
    createMenuBar();
    bindMenuBar();
    allocateMem();
    overlayPanels();
    initParams();
    initDimensions();
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onQuitFrame, this);
}

MainFrame::MainFrame(const wxString &initImg): MainFrame() {
    openImage(initImg);
}

MainFrame::~MainFrame() {

}

void MainFrame::initDimensions() {
    minMainSplitterSize =
        wxSystemSettings::GetMetric(wxSYS_SCREEN_X) / mainSplitterFactor;
    minSideSplitterSize =
        wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) / sideSplitterFactor;
    mainSplitter->SetMinimumPaneSize(minMainSplitterSize);
    SetMinClientSize(wxSize(minMainSplitterSize * (mainSplitterFactor - 1),
                            minSideSplitterSize * (sideSplitterFactor - 1)));
}

void MainFrame::allocateMem() {
    mainSplitter = new wxSplitterWindow(this, ict::MAIN_SPLITTER);
    sCanvas = new ExtendedCanvas(mainSplitter, ict::SCVIEW);
    // sCanvas->useGrid(true);
    wxRect2DDouble res(0, 0, 200, 200);
    CanvasItem *it1 = new CanvasItem(1, res);
    it1->lock(false);
    sCanvas->addItem(it1);
    CanvasItem *it2 = new CanvasItem(2, wxRect2DDouble(0, 0, 40, 80));
    it2->lock(false);
    // it2->setVirtualRestriction(res);
    // it2->restrict(true);
    // it2->fixedAspectRatio(true);
    // it2->expandFromCenter(true);
    sCanvas->addItem(it2);
    tools = new ToolsPanel(mainSplitter, ict::TOOLS);
    mainSizer = new wxBoxSizer(wxVERTICAL);
}

void MainFrame::createMenuBar() {
    topMenuBar = new wxMenuBar;

    mFile = new wxMenu;
    mFile->Append(wxID_OPEN);
    mFile->Append(wxID_CLOSE);
    mFile->AppendSeparator();
    mFile->Append(ict::EXPORT_MI, "Export");
    mFile->AppendSeparator();
    mFile->Append(wxID_EXIT);

    mHelp = new wxMenu;
    mHelp->Append(wxID_HELP);
    mHelp->AppendSeparator();
    mHelp->Append(wxID_ABOUT);

    topMenuBar->Append(mFile, "File");
    topMenuBar->Append(mHelp, "More");

    SetMenuBar(topMenuBar);
}

void MainFrame::overlayPanels() {
    mainSplitter->SplitVertically(tools, sCanvas);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizerAndFit(mainSizer);
}

void MainFrame::bindMenuBar() {
    Bind(wxEVT_MENU, &MainFrame::onOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MainFrame::onExport, this, ict::EXPORT_MI);
    Bind(wxEVT_MENU, &MainFrame::onQuit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainFrame::onClose, this, wxID_CLOSE);
    Bind(wxEVT_MENU, &MainFrame::onAbout, this, wxID_ABOUT);
}

void MainFrame::bindElements() {
//    sView->
//        getCanvas()->Bind(EVT_CROP_CHANGE, &MainFrame::onCropChange, this);
    Bind(wxEVT_BUTTON, &MainFrame::resetCrop, this, ict::RESET_CROP_BT);
    tools->
        Bind(wxEVT_CHECKBOX, &MainFrame::onFixRatio, this, ict::FIX_RATIO_CB);
    tools->
        Bind(wxEVT_CHECKBOX, &MainFrame::onAllowGrow, this,
             ict::GROW_CHECK_CB);
}

void MainFrame::unbindElements() {
//    sView->
//        getCanvas()->Unbind(EVT_CROP_CHANGE, &MainFrame::onCropChange, this);
    Unbind(wxEVT_BUTTON, &MainFrame::resetCrop, this, ict::RESET_CROP_BT);
    tools->
        Unbind(wxEVT_CHECKBOX, &MainFrame::onFixRatio, this,
               ict::FIX_RATIO_CB);
    tools->
        Unbind(wxEVT_CHECKBOX, &MainFrame::onAllowGrow, this,
               ict::GROW_CHECK_CB);
}

void MainFrame::onAbout(wxCommandEvent &event) {
    std::string aboutTxt = projectName;
    aboutTxt += " - v";
    aboutTxt += versionString;
    aboutTxt += "\nby manpaco";
    wxMessageBox(aboutTxt, _("About"), wxOK | wxICON_INFORMATION);
}

void MainFrame::onClose(wxCommandEvent &event) {
    if(!openedImg) return;
    if(!exportedImg) {
        if(showProceedMessage() != wxYES) return;
    }
    clear();
}

void MainFrame::onOpen(wxCommandEvent &event) {
    if(openedImg && !exportedImg) {
        if(showProceedMessage() != wxYES) return;
    }
    wxFileDialog openDlg(this, _("Open image"), "", "", _(importWc),
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(openDlg.ShowModal() == wxID_CANCEL) return;
    openImage(openDlg.GetPath());
}

void MainFrame::onExport(wxCommandEvent &event) {
    if(!openedImg) return;
    ExportDialog expImg(this);
    if(expImg.ShowModal() == wxID_CANCEL) return;
    exportImage(expImg.validPath());
}

void MainFrame::onQuit(wxCommandEvent &event) {
    Close(false);
}

void MainFrame::onQuitFrame(wxCloseEvent &event) {
    if(event.CanVeto() && openedImg) {
        if(!exportedImg) {
            if(showProceedMessage() != wxYES) {
                event.Veto();
                return;
            }
        } else {
            if(showCloseMessage() != wxYES) {
                event.Veto();
                return;
            }
        }
    }
    event.Skip();
}

int MainFrame::showCloseMessage() {
    return wxMessageBox(_("There is an image in the workspace! Close anyway?"),
                        _("Please confirm"), wxYES_NO, this);
}

int MainFrame::showProceedMessage() {
    return wxMessageBox(_("Current content has not been exported! Proceed?"),
                        _("Please confirm"), wxICON_QUESTION | wxYES_NO, this);
}

void MainFrame::exportImage(const wxString &p) {
    if(!openedImg) return;
//    try {
//        Magick::Image out(composeState(*sourceImg, currentState));
//        out.write(p.ToStdString());
//        exportedImg = true;
//    }
//    catch(Magick::Exception &e) {
//        wxMessageBox(e.what(), "Error exporting image");
//    }
}

void MainFrame::openImage(const wxString &p) {
    if(openedImg) clear();
//    try {
//        sourceImg = new Magick::Image(p.ToStdString());
//        compImg = new Magick::Image(*sourceImg);
//        sView->handle(compImg);
//        wxBitmap newBmp(createImage(*compImg));
//        preview->updatePreview(newBmp);
//        bindElements();
//        tools->clear(true);
//        tools->cropSize(sView->getCanvas()->cropSize());
//        currentState = tools->currentOpts();
//        mEdit->Enable(wxID_APPLY, true);
//        openedImg = true;
//    }
//    catch(Magick::Exception &e) {
//        std::cerr << e.what() << std::endl;
//        wxMessageBox(e.what(), "Error opening image");
//    }
}

void MainFrame::initParams() {
//    currentState = OptionsContainer();
    openedImg = false;
    exportedImg = false;
}

void MainFrame::clear() {
    unbindElements();
//    if(sourceImg) {
//        delete sourceImg;
//        sourceImg = nullptr;
//    }
//    if(compImg) {
//        delete compImg;
//        compImg = nullptr;
//    }
//    undoStack = std::stack<OptionsContainer>();
//    redoStack = std::stack<OptionsContainer>();
    initParams();
    tools->Enable(false);
    tools->collapseBlocks();
//    sView->clear();
}

void MainFrame::onFixRatio(wxCommandEvent &event) {
//    sView->getCanvas()->fixCrop(event.IsChecked());
    event.Skip();
}

void MainFrame::onAllowGrow(wxCommandEvent &event) {
//    if(sView->getCanvas()->allowGrow(event.IsChecked()))
//        tools->cropGeometry(sView->getCanvas()->cropGeometry());
    event.Skip();
}

void MainFrame::resetCrop(wxCommandEvent &event) {
//    wxRect rst(wxPoint(0, 0), wxSize(-1, -1));
//    sView->getCanvas()->cropGeometry(&rst);
//    tools->cropGeometry(rst);
}

void MainFrame::onCropChange(CropEvent &event) {
    wxRect newGeometry(event.getOffset(), event.getSize());
    tools->cropGeometry(newGeometry);
}

void MainFrame::composePreview() {
//    OptionsContainer aux(currentState);
//    aux.cropSize =
//        sView->getCanvas()->
//            translateSize(aux.cropSize, ict::COMPRESS_T, ict::IN_D);
//    aux.cropOff =
//        sView->getCanvas()->
//            translatePoint(aux.cropOff, ict::COMPRESS_T, ict::IN_D);
//    Magick::Image newImg = composeState(*compImg, aux);
//    wxBitmap newPreview(createImage(newImg));
//    preview->updatePreview(newPreview);
}
