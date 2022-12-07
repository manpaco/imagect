#include "mainframe.h"
#include "Magick++.h"
#include "canvaspanel.h"
#include "cropevent.h"
#include "identifiersdef.h"
#include "previewpanel.h"
#include "toolspanel.h"
#include "wx/splitter.h"
#include "imgtools.h"

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
    mFile->AppendSeparator();
    mFile->Append(ict::EXPORT_MI, "Export");
    mFile->AppendSeparator();
    mFile->Append(wxID_CLOSE);
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

wxBitmap MainFrame::createBitmap(Magick::Image &img) {
    return wxBitmap(wxImage(img.columns(), img.rows(), 
            extractRgb(img), extractAlpha(img)));
}

void MainFrame::overlayPanels() {
    sideSplitter->SplitHorizontally(preview, tools);
    mainSplitter->SplitVertically(sideSplitter, canvas);
    wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->AddSpacer(bestSpace);
    buttonsSizer->Add(apply);
    buttonsSizer->AddSpacer(bestSpace);
    buttonsSizer->Add(reset);
    mainSizer->Add(buttonsSizer);
    mainSizer->AddSpacer(bestSpace);
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
        if(showProceedMessage() == wxNO) return;
    }
    clear();
}

void MainFrame::onOpen(wxCommandEvent &event) {
    if(openedImg && !exportedImg) {
        if(showProceedMessage() == wxNO) return;
    }
    wxFileDialog openDlg(this, _("Open image"), "", "", 
            _("PNG (*.png)|*.png|JPEG (*.jpeg;*.jpg)|*jpeg;*.jpg"), 
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(openDlg.ShowModal() == wxID_CANCEL) return;
    openImage(openDlg.GetPath());
}

void MainFrame::onExport(wxCommandEvent &event) {
    if(!openedImg) return;
    exportImage();
}

void MainFrame::onQuit(wxCommandEvent &event) {
    Close(false);
}

void MainFrame::onQuitFrame(wxCloseEvent &event) {
    if(event.CanVeto() && openedImg) {
        if(!exportedImg) {
            if(showProceedMessage() == wxNO) {
                event.Veto();
                return;
            }
        } else {
            if(showCloseMessage() == wxNO) {
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

void MainFrame::exportImage() {
    wxFileDialog exportDlg(this, _("Export image"),wxEmptyString, wxEmptyString, 
            "PNG (*.png)|*.png|JPEG (*.jpeg;*.jpg)|*jpeg;*.jpg", 
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(exportDlg.ShowModal() == wxID_CANCEL) return;
    Magick::Image out(composeState(*sourceImg, *currentState));
    out.write(exportDlg.GetPath().ToStdString());
    exportedImg = true;
}

void MainFrame::openImage(const wxString &p) {
    if(openedImg) {
        delete sourceImg;
        delete scaledImg;
        history.clear();
        currentState = history.begin();
    } else bindElements();
    sourceImg = new Magick::Image(p.ToStdString());
    scaledImg = new Magick::Image(*sourceImg);
    scaledImg->zoom(Magick::Geometry(scaledImg->columns() * 0.3, scaledImg->rows() * 0.3));
    wxBitmap newBmp = createBitmap(*scaledImg);
    preview->updatePreview(newBmp);
    canvas->updateCanvas(newBmp);
    tools->clear(true);
    tools->cropSize(canvas->cropSize());
    OptionsContainer opts = tools->currentOpts();
    State toSave = std::make_tuple(canvas->getCropOffset(), opts);
    updateHistory(toSave);
    mEdit->Enable(wxID_APPLY, true);
    openedImg = true;
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
    tools->clear(false);
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
    tools->setOpts(std::get<1>(*currentState));
    updateCropGeometry(*currentState);
    if(currentState == history.begin()) mEdit->Enable(wxID_UNDO, false);
    mEdit->Enable(wxID_REDO, true);
    composePreview();
    exportedImg = false;
}

void MainFrame::updateCropGeometry(State &s) {
    wxRect g(std::get<0>(s), std::get<1>(s).cropSize);
    canvas->cropGeometry(g);
}

void MainFrame::redo(wxCommandEvent &event) {
    if(currentState == history.begin()) mEdit->Enable(wxID_UNDO, true);
    currentState++;
    tools->setOpts(std::get<1>(*currentState));
    updateCropGeometry(*currentState);
    if(currentState == --history.end()) mEdit->Enable(wxID_REDO, false);
    composePreview();
    exportedImg = false;
}

void MainFrame::updateHistory(State toSave) {
    if(currentState != history.end()) history.erase(++currentState, history.end());
    history.push_back(toSave);
    currentState = --history.end();
    mEdit->Enable(wxID_REDO, false);
    if(currentState == ++history.begin()) mEdit->Enable(wxID_UNDO, true);
    composePreview();
    exportedImg = false;
}

void MainFrame::onCropChange(CropEvent &event) {
    if(tools->cropSize() != event.getSize()) tools->cropSize(event.getSize());
}

Magick::Image MainFrame::composeState(const Magick::Image &img, const State &s) {
    int w = std::get<1>(s).cropSize.GetWidth();
    int h = std::get<1>(s).cropSize.GetHeight();
    int xo = std::get<0>(s).x;
    int yo = std::get<0>(s).y;
    Magick::Geometry crop(w, h, xo, yo);
    Magick::Image comp(extractArea(crop, img));
    if(std::get<1>(s).allowGrow) {
        //Magick::Image back(img.geometry(), Magick::Color(0, 0, 0, QuantumRange / 2));
        //if(std::get<1>(s).growChoice == ict::IMAGE) 
        //    if(!std::get<1>(s).backImage.ToStdString().empty())
        //        back.read(std::get<1>(s).backImage.ToStdString());
        //overlap(comp, back, true, true);
        //return back;
    }
    return comp;
}

void MainFrame::composePreview() {
    Magick::Image newImg = composeState(*scaledImg, *currentState);
    wxBitmap newPreview = createBitmap(newImg);
    updatePreview(newPreview);
}

void MainFrame::saveState(wxCommandEvent &event) {
    // update preview and/or update crop rectangle
    canvas->cropSize(tools->cropSize());
    State toSave = std::make_tuple(canvas->getCropOffset(), tools->currentOpts());
    if(toSave == *currentState) return;
    updateHistory(toSave);
}

void MainFrame::updatePreview(wxBitmap &bm) {
    preview->updatePreview(bm);
}
