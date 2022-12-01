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

MainFrame::MainFrame(): wxFrame(NULL, wxID_ANY, "Image Croping Tool") {
    allocateMem();
    overlayPanels();
    setBindings();
}

MainFrame::~MainFrame() {
    if(sourceImg) delete(sourceImg);
    if(lowResImg) delete(lowResImg);
    if(dumpBitmap) delete(dumpBitmap);
}

void MainFrame::allocateMem() {
    sourceImg = new Magick::Image("/usr/share/icons/Adwaita/512x512/devices/computer.png");
    lowResImg = new Magick::Image(*sourceImg);
    Magick::Geometry newRes(lowResImg->columns() * 0.3, lowResImg->rows() * 0.3);
    lowResImg->zoom(newRes);
    dumpBitmap = createBitmap(*lowResImg);
    mainSplitter = new wxSplitterWindow(this, ict::MAIN_SPLITTER);
    sideSplitter = new wxSplitterWindow(mainSplitter, ict::SIDE_SPLITTER);
    canvas = new CanvasPanel(mainSplitter, ict::CANVAS, *dumpBitmap);
    tools = new ToolsPanel(sideSplitter, ict::TOOLS);
    preview = new PreviewPanel(sideSplitter, ict::PREVIEW);
    mainSizer = new wxBoxSizer(wxHORIZONTAL);
    updatePreview();
}

wxBitmap * MainFrame::createBitmap(Magick::Image &img) {
    return new wxBitmap(wxImage(img.columns(), img.rows(), 
            extractRgb(img), extractAlpha(img)));
}

void MainFrame::overlayPanels() {
    sideSplitter->SplitHorizontally(tools, preview);
    mainSplitter->SplitVertically(canvas, sideSplitter);
    mainSplitter->SetMinimumPaneSize(150);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizerAndFit(mainSizer);
}

void MainFrame::setBindings() {
    tools->Bind(wxEVT_BUTTON, &MainFrame::applyChanges, this, ict::APPLY_BT);
    canvas->Bind(EVT_CROP_CHANGE, &MainFrame::onCropChange, this);
}

void MainFrame::onCropChange(CropEvent &event) {
    // update preview
    Magick::Geometry crop(event.getSize().GetWidth(), event.getSize().GetHeight(), event.getOffset().x, event.getOffset().y);
    Magick::Image ext(extractArea(crop, *lowResImg));
    if(dumpBitmap) delete dumpBitmap;
    dumpBitmap = createBitmap(ext);
    updatePreview();

    std::cout << ext.columns() << " - " << ext.rows() << std::endl;
    std::cout << dumpBitmap->GetWidth() << " - " << dumpBitmap->GetHeight() << std::endl;
}

void MainFrame::applyChanges(wxCommandEvent &event) {
    // update preview and/or update crop rectangle
    OptionsContainer attribs = tools->currentStatus();
    
}

void MainFrame::updatePreview() {
    preview->updatePreview(*dumpBitmap);
}
