#include "MetalinkEditor.hpp"
#include "Metalink4Writer.hpp"
#include "Metalink3Writer.hpp"
#include "Metalink4Reader.hpp"
#include "Metalink3Reader.hpp"
#include <wx/filename.h>

MetalinkEditor::MetalinkEditor()
{
    selection_ = 0;
}

bool MetalinkEditor::is_empty() const
{
    return metalink_.is_empty();
}

int MetalinkEditor::num_files() const
{
    return metalink_.num_files();
}

const wxString& MetalinkEditor::get_filename(int file) const
{
    return metalink_.get_file(file).get_filename();
}

void MetalinkEditor::add_file(const wxString& filename)
{
    add_file(MetalinkFile(filename));
}

void MetalinkEditor::add_file(const MetalinkFile& file)
{
    metalink_.add_file(file);
    selection_ = metalink_.num_files() - 1;
    update();
}

void MetalinkEditor::add_listener(MetalinkEditorListener* listener)
{
    listeners_.push_back(listener);
}

void MetalinkEditor::select(int file)
{
    if(file < 0 || file >= metalink_.num_files()) return;
    selection_ = file;
    update();
}

int MetalinkEditor::get_selection() const
{
    return selection_;
}

void MetalinkEditor::remove_file()
{
    // Remove the file
    metalink_.remove_file(selection_);
    // Fix selection
    if(metalink_.is_empty()) {
        selection_ = 0;
    } else if(selection_ >= metalink_.num_files()) {
        selection_ = metalink_.num_files() - 1;
    }
    // Update
    update();
}

const MetalinkFile& MetalinkEditor::get_file() const
{
    return metalink_.get_file(selection_);
}

const std::vector<MetalinkFile>& MetalinkEditor::get_files() const
{
    return metalink_.get_files();
}

void MetalinkEditor::set_file(const MetalinkFile& file)
{
    metalink_.set_file(selection_, file);
    update();
}

const wxString& MetalinkEditor::get_filename() const
{
    return filename_;
}

void MetalinkEditor::set_filename(const wxString& filename)
{
    filename_ = filename;
}

void MetalinkEditor::update()
{
    for(int i = 0; i < listeners_.size(); i++) {
        listeners_.at(i)->update();
    }
}

void MetalinkEditor::save()
{
    if(filename_.empty()) return;
    if(wxFileName(filename_).GetExt() == wxT("metalink")) {
        Metalink3Writer writer(*this);
        writer.save(filename_);
    } else {
        Metalink4Writer writer(*this);
        writer.save(filename_);
    }
}

bool MetalinkEditor::load_metalink4(const wxString& filename)
{
    Metalink4Reader reader(*this);
    return reader.load(filename);
}

bool MetalinkEditor::load_metalink3(const wxString& filename)
{
    Metalink3Reader reader(*this);
    return reader.load(filename);
}

void MetalinkEditor::open(const wxString& filename)
{
    try {
        bool loaded = load_metalink4(filename);
        if(!loaded) {
            loaded = load_metalink3(filename);
        }
        if(!loaded) {
            throw MetalinkLoadError("Unrecognized file format!");
        }
        filename_ = filename;
    } catch(MetalinkLoadError& e) {
        clear();
        throw e;
    }
}

void MetalinkEditor::clear()
{
    metalink_.clear();
    filename_.clear();
    selection_ = 0;
    update();
}
