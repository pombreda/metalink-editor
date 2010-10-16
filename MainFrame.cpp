#include "MainFrame.hpp"
#include "LicenseFrame.hpp"
#include "util.hpp"
#include "common.hpp"
#include <wx/filename.h>
#include <wx/aboutdlg.h>
#include <iostream>

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_Quit, MainFrame::on_quit)
    EVT_MENU(ID_About, MainFrame::on_about)
    EVT_MENU(ID_License, MainFrame::on_license)
    EVT_MENU(ID_New, MainFrame::on_new)
    EVT_MENU(ID_AddFile, MainFrame::on_add_file)
    EVT_MENU(ID_DelFile, MainFrame::on_del_file)
    EVT_CHOICE(ID_FileChoice, MainFrame::on_file_select)
END_EVENT_TABLE()

MainFrame::MainFrame()
    : wxFrame((wxFrame *)0, -1, wxT(""), wxDefaultPosition, wxDefaultSize)
{
    create_menu();
    create_widgets();
    update_start(true);
    // Set properties
    SetTitle(wxT("Metalink Editor"));
    SetSize(wxSize(420, 412));
    // Update interface
    update();
    editor_.add_listener(this);
}

void MainFrame::create_menu()
{
    wxMenuBar* main_menubar = new wxMenuBar();
    wxMenu* menu_file = new wxMenu();
    menu_file->Append(ID_New, wxT("New"), wxEmptyString, wxITEM_NORMAL);
    menu_file->AppendSeparator();
    menu_file->Append(ID_Quit, wxT("Exit"), wxEmptyString, wxITEM_NORMAL);
    main_menubar->Append(menu_file, wxT("File"));
    wxMenu* menu_metalink = new wxMenu();
    menu_metalink->Append(ID_AddFile, wxT("Add empty file..."), wxEmptyString, wxITEM_NORMAL);
    menu_metalink->Append(ID_DelFile, wxT("Remove file..."), wxEmptyString, wxITEM_NORMAL);
    main_menubar->Append(menu_metalink, wxT("Metalink"));
    wxMenu* menu_help = new wxMenu();
    menu_help->Append(ID_License, wxT("License"), wxEmptyString, wxITEM_NORMAL);
    menu_help->Append(ID_About, wxT("About"), wxEmptyString, wxITEM_NORMAL);
    main_menubar->Append(menu_help, wxT("Help"));
    SetMenuBar(main_menubar);
}

void MainFrame::create_widgets()
{
    // Create widgets
    file_choice_ = new wxChoice(this, ID_FileChoice, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    notebook_ = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    // Layout
    wxBoxSizer* sizer1 = new wxBoxSizer(wxVERTICAL);
    sizer1->Add(file_choice_, 0, wxEXPAND, 0);
    sizer1->Add(notebook_, 1, wxEXPAND, 0);
    SetSizerAndFit(sizer1);
    Layout();
}

// Show start page if editor is empty and otherwise the regular interface.
void MainFrame::update_start(bool force)
{
    if(force || (!start_ && editor_.is_empty())) {
        // Switch to start page
        notebook_->DeleteAllPages();
        wxPanel* notebook_page1 = new wxPanel(notebook_, wxID_ANY);
        wxStaticText* label1 = new wxStaticText(notebook_page1, wxID_ANY, wxT("Welcome to Metalink Editor 2\n\nThis is an empty metalink. Get started by either opening\nan existing metalink or adding a file to this one. You can\nadd a file from the metalink menu."), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
        notebook_->AddPage(notebook_page1, wxT("Start"));
        wxBoxSizer* sizer2 = new wxBoxSizer(wxVERTICAL);
        sizer2->Add(label1, 1, wxALL|wxEXPAND, 10);
        notebook_page1->SetSizer(sizer2);
        start_ = true;
    } else if(force || (start_ && !editor_.is_empty())) {
        // Switch to regular user interface
        notebook_->DeleteAllPages();
        wxPanel* notebook_page2 = new wxPanel(notebook_, wxID_ANY);
        notebook_->AddPage(notebook_page2, wxT("Sources"));
        start_ = false;
    }
}

void MainFrame::update()
{
    update_start();
    // Update file choice
    file_choice_->Clear();
    for(int i = 0; i < editor_.num_files(); i++) {
        file_choice_->Append(editor_.get_filename(i));
    }
    file_choice_->SetSelection(editor_.get_selection());
}

void MainFrame::on_quit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MainFrame::on_about(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo info;
    info.SetName(_T("Metalink Editor"));
    info.SetVersion(_T("2.0dev"));
    info.SetDescription(_T("This is a preview of Metalink Editor 2.0"));
    info.AddDeveloper(_T("Hampus Wessman"));
    wxAboutBox(info);
}

void MainFrame::on_license(wxCommandEvent& WXUNUSED(event))
{
    LicenseFrame* frame = new LicenseFrame();
    frame->Show(true);
}

void MainFrame::on_add_file(wxCommandEvent& WXUNUSED(event))
{
    wxString filename = wxGetTextFromUser(
        wxT("Please enter a file name:"),
        wxT("Add empty file to metalink")
    );
    if(filename == wxT("")) return;
    editor_.add_file(filename);
}

void MainFrame::on_del_file(wxCommandEvent& WXUNUSED(event))
{
    if(editor_.is_empty()) return;
    int answer = wxMessageBox(
        wxT("Remove currently selected file from metalink?"),
        wxT("Remove file"),
        wxOK | wxCANCEL | wxICON_EXCLAMATION
    );
    if(answer == wxCANCEL) return;
    editor_.remove_file();
}

void MainFrame::on_file_select(wxCommandEvent& event)
{
    editor_.select(event.GetInt());
}

void MainFrame::on_new(wxCommandEvent& WXUNUSED(event))
{
    int answer = wxMessageBox(
        wxT("Remove all currently loaded data and start from scratch?"),
        wxT("New metalink"),
        wxOK | wxCANCEL | wxICON_EXCLAMATION
    );
    if(answer == wxCANCEL) return;
    editor_.clear();
}
