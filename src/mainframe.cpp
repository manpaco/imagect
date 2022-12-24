#include "mainframe.h"
#include "canvaspanel.h"
#include "cropevent.h"
#include "defs.h"
#include "previewpanel.h"
#include "optscontainer.h"
#include "toolspanel.h"
#include "imgtools.h"
#include "filext.h"
#include "exportdlg.h"
#include <Magick++.h>

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

using Magick::Quantum;

extern const int bestSpace = 5;

MainFrame::MainFrame(): 
    wxFrame(NULL, wxID_ANY, "Image Cropping Tool", wxDefaultPosition, 
            wxDefaultSize, wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | 
            wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | 
            wxMAXIMIZE) {
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
    if(sourceImg) delete(sourceImg);
    if(scaledImg) delete(scaledImg);
}

void MainFrame::initDimensions() {
    minMainSplitterSize = wxSystemSettings::GetMetric(wxSYS_SCREEN_X) / mainSplitterFactor;
    minSideSplitterSize = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) / sideSplitterFactor;
    mainSplitter->SetMinimumPaneSize(minMainSplitterSize);
    sideSplitter->SetMinimumPaneSize(minSideSplitterSize);
    SetMinClientSize(wxSize(minMainSplitterSize * 3.0, minSideSplitterSize * 2.5));
}

void MainFrame::allocateMem() {
    mainSplitter = new wxSplitterWindow(this, ict::MAIN_SPLITTER);
    sideSplitter = new wxSplitterWindow(mainSplitter, ict::SIDE_SPLITTER);
    canvas = new CanvasPanel(mainSplitter, ict::CANVAS);
    tools = new ToolsPanel(sideSplitter, ict::TOOLS);
    preview = new PreviewPanel(sideSplitter, ict::PREVIEW);
    mainSizer = new wxBoxSizer(wxVERTICAL);
    apply = new wxButton(this, ict::APPLY_BT, "Apply");
    reset = new wxButton(this, ict::RESET_CROP_BT, "Reset crop area");
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

    mEdit = new wxMenu;
    mEdit->Append(wxID_UNDO);
    mEdit->Append(wxID_REDO);
    mEdit->Append(wxID_APPLY);

    mHelp = new wxMenu;
    mHelp->Append(wxID_HELP);
    mHelp->AppendSeparator();
    mHelp->Append(wxID_ABOUT);

    topMenuBar->Append(mFile, "File");
    topMenuBar->Append(mEdit, "Edit");
    topMenuBar->Append(mHelp, "More");
    
    SetMenuBar(topMenuBar);
}

void MainFrame::overlayPanels() {
    sideSplitter->SplitHorizontally(preview, tools);
    mainSplitter->SplitVertically(sideSplitter, canvas);
    wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->AddSpacer(bestSpace);
    buttonsSizer->Add(apply);
    buttonsSizer->AddSpacer(bestSpace);
    buttonsSizer->Add(reset);
    buttonsSizer->AddStretchSpacer();
    buttonsSizer->Add(new wxButton(this, wxID_ANY, "Zoom"));
    buttonsSizer->AddSpacer(bestSpace);
    mainSizer->AddSpacer(bestSpace);
    mainSizer->Add(buttonsSizer, 0, wxEXPAND);
    mainSizer->AddSpacer(bestSpace);
    wxStaticLine *hLine = new wxStaticLine(this);
    mainSizer->Add(hLine, 0, wxEXPAND);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizerAndFit(mainSizer);
}

void MainFrame::bindMenuBar() {
    Bind(wxEVT_MENU, &MainFrame::undo, this, wxID_UNDO);
    Bind(wxEVT_MENU, &MainFrame::redo, this, wxID_REDO);
    Bind(wxEVT_MENU, &MainFrame::onOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MainFrame::onExport, this, ict::EXPORT_MI);
    Bind(wxEVT_MENU, &MainFrame::onQuit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainFrame::saveState, this, wxID_APPLY);
    Bind(wxEVT_MENU, &MainFrame::onClose, this, wxID_CLOSE);
    Bind(wxEVT_MENU, &MainFrame::onAbout, this, wxID_ABOUT);
}

void MainFrame::bindElements() {
    canvas->Bind(EVT_CROP_CHANGE, &MainFrame::onCropChange, this);
    Bind(wxEVT_BUTTON, &MainFrame::saveState, this, ict::APPLY_BT);
    Bind(wxEVT_BUTTON, &MainFrame::resetCrop, this, ict::RESET_CROP_BT);
    tools->Bind(wxEVT_CHECKBOX, &MainFrame::onFixRatio, this, ict::FIX_RATIO_CB);
    tools->Bind(wxEVT_CHECKBOX, &MainFrame::onAllowGrow, this, ict::GROW_CHECK_CB);
}

void MainFrame::unbindElements() {
    canvas->Unbind(EVT_CROP_CHANGE, &MainFrame::onCropChange, this);
    Unbind(wxEVT_BUTTON, &MainFrame::saveState, this, ict::APPLY_BT);
    Unbind(wxEVT_BUTTON, &MainFrame::resetCrop, this, ict::RESET_CROP_BT);
    tools->Unbind(wxEVT_CHECKBOX, &MainFrame::onFixRatio, this, ict::FIX_RATIO_CB);
    tools->Unbind(wxEVT_CHECKBOX, &MainFrame::onAllowGrow, this, ict::GROW_CHECK_CB);
}

void MainFrame::resetCrop(wxCommandEvent &event) {
    canvas->cropGeometry(wxRect(wxPoint(0, 0), wxSize(-1, -1)));
}

void MainFrame::onAbout(wxCommandEvent &event) {
    wxMessageBox(_("Image Cropping Tool\rby manpaco"), _("About"), wxOK | wxICON_INFORMATION);
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
    try {
        Magick::Image out(composeState(*sourceImg, *currentState));
        out.write(p.ToStdString());
        exportedImg = true;
    }
    catch(Magick::Exception &e) {
        wxMessageBox(e.what(), "Error exporting image");
    }
}

void MainFrame::openImage(const wxString &p) {
    if(openedImg) clear();
    try {
        sourceImg = new Magick::Image(p.ToStdString());
        scaledImg = new Magick::Image(*sourceImg);
        scaledImg->zoom(Magick::Geometry(scaledImg->columns() * 0.3, scaledImg->rows() * 0.3));
        wxBitmap newBmp(createImage(*scaledImg));
        bindElements();
        preview->updatePreview(newBmp);
        canvas->updateCanvas(newBmp);
        tools->clear(true);
        tools->cropSize(canvas->cropSize());
        OptionsContainer toSave = tools->currentOpts();
        updateHistory(toSave);
        mEdit->Enable(wxID_APPLY, true);
        openedImg = true;
    }
    catch(Magick::Exception &e) {
        std::cerr << e.what() << std::endl;
        wxMessageBox(e.what(), "Error opening image");
    }
}

void MainFrame::initParams() {
    currentState = history.begin();
    openedImg = false;
    exportedImg = false;
    mEdit->Enable(wxID_APPLY, false);
    mEdit->Enable(wxID_UNDO, false);
    mEdit->Enable(wxID_REDO, false);
}

void MainFrame::clear() {
    unbindElements();
    if(sourceImg) {
        delete sourceImg;
        sourceImg = nullptr;
    }
    if(scaledImg) {
        delete scaledImg;
        scaledImg = nullptr;
    }
    history.clear();
    initParams();
    tools->Enable(false);
    tools->collapseBlocks();
    canvas->clear();
    preview->clear();
}

void MainFrame::onFixRatio(wxCommandEvent &event) {
    canvas->fixCrop(event.IsChecked());
    event.Skip();
}

void MainFrame::onAllowGrow(wxCommandEvent &event) {
    canvas->allowGrow(event.IsChecked());
    event.Skip();
}

void MainFrame::undo(wxCommandEvent &event) {
    currentState--;
    tools->setOpts(*currentState);
    updateCropGeometry(currentState->cropOff, currentState->cropSize);
    if(currentState == history.begin()) mEdit->Enable(wxID_UNDO, false);
    mEdit->Enable(wxID_REDO, true);
    composePreview();
    exportedImg = false;
}

void MainFrame::updateCropGeometry(wxPoint &o, wxSize &s) {
    wxRect g(o, s);
    canvas->cropGeometry(g);
}

void MainFrame::redo(wxCommandEvent &event) {
    if(currentState == history.begin()) mEdit->Enable(wxID_UNDO, true);
    currentState++;
    tools->setOpts(*currentState);
    updateCropGeometry(currentState->cropOff, currentState->cropSize);
    if(currentState == --history.end()) mEdit->Enable(wxID_REDO, false);
    composePreview();
    exportedImg = false;
}

void MainFrame::updateHistory(OptionsContainer toSave) {
    if(currentState != history.end()) history.erase(++currentState, history.end());
    history.push_back(toSave);
    currentState = --history.end();
    mEdit->Enable(wxID_REDO, false);
    if(currentState == ++history.begin()) mEdit->Enable(wxID_UNDO, true);
    composePreview();
    exportedImg = false;
}

void MainFrame::onCropChange(CropEvent &event) {
    wxRect newGeometry(event.getOffset(), event.getSize());
    tools->cropGeometry(newGeometry);
}

void MainFrame::composePreview() {
    Magick::Image newImg = composeState(*scaledImg, *currentState);
    wxBitmap newPreview(createImage(newImg));
    preview->updatePreview(newPreview);
}

void MainFrame::saveState(wxCommandEvent &event) {
    if(!tools->checkValues()) return;
    canvas->cropSize(tools->optsCropSize());
    OptionsContainer toSave = tools->currentOpts();
    if(toSave == *currentState) return;
    updateHistory(toSave);
}

