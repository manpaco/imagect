#include "mainframe.h"
#include "Magick++.h"
#include "canvaspanel.h"
#include "defs.h"
#include "previewframe.h"
#include "toolspanel.h"
#include "wx/splitter.h"

using Magick::Quantum;

MainFrame::MainFrame(): wxFrame(NULL, wxID_ANY, "Image Croping Tool") {
    allocateMem();
    overlayPanels();
    setBindings();
}

void MainFrame::allocateMem() {
    mainSplitter = new wxSplitterWindow(this, ict::SPLITTER);
    canvas = new CanvasPanel(mainSplitter, ict::CANVAS);
    tools = new ToolsPanel(mainSplitter, ict::TOOLS);
    preview = nullptr;
    mainSizer = new wxBoxSizer(wxHORIZONTAL);
}

void MainFrame::overlayPanels() {
    mainSplitter->SplitVertically(canvas, tools);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizerAndFit(mainSizer);
}

void MainFrame::setBindings() {
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onExit, this);
    //tools->Bind(wxEVT_BUTTON, &MainFrame::showPreview, this, SHOW_PREVIEW_BT);
    tools->Bind(wxEVT_CHECKBOX, &MainFrame::changeGrowState, this, 
            ict::GROW_CHECK_CB);
    tools->Bind(wxEVT_RADIOBOX, &MainFrame::changeGrowChoice, this, 
            ict::GROW_SELECTOR_RB);
}

void MainFrame::changeGrowState(wxCommandEvent &event) {
    tools->growState(event.IsChecked());
    // change grow option in canvas
}

void MainFrame::changeGrowChoice(wxCommandEvent &event) {
    tools->growChoice(event.GetSelection());
    // set grow background
}

void MainFrame::onExit(wxCloseEvent &event) {
    if(preview) preview->Destroy();
    if(tools) tools->Destroy();
    if(canvas) canvas->Destroy();
    if(mainSplitter) mainSplitter->Destroy();
    if(imgTest) delete(imgTest);
    Destroy();
}

void MainFrame::closePreview(wxCloseEvent &event) {
    preview->Destroy();
    preview = nullptr;
}

void MainFrame::showPreview(wxCommandEvent &event) {
    if(preview){
        if(preview->IsShown()) {
            preview->Raise();
            return;
        }
    } else {
        if(imgTest) createPreview(*imgTest);
        else return;
    }
    preview->Show();
}

void MainFrame::createPreview(Magick::Image &img) {
    preview = new PreviewFrame(this, ict::PREVIEW, "Preview", img);
    preview->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::closePreview, this);
}

