#pragma once

#include "colorpicker.hpp"
#include "editor.hpp"
#include "shortcuts.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/screen/box.hpp>
#include <unordered_map>

namespace Termisprite
{


class Modal
    : public ftxui::ComponentBase
{
public:
    Modal( std::function<void()> onClose, std::string const& title = "" );

    virtual ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

protected:
    void initTree();

    virtual ftxui::Element renderModalContent() { return {}; };
    virtual ftxui::Component buildContentComponent() const { return ftxui::Container::Vertical({}); };
    virtual void onConfirm() { M_closeCallback(); };

protected:
    std::function<void()> M_closeCallback;
    ftxui::Component M_cancelButton;
    ftxui::Component M_okButton;

    ftxui::Box M_box;
    std::string M_title;

};


class NewProjectModal
    : public Modal
{
public:
    NewProjectModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose )
        : Modal( onClose, "New Project"), M_editorCanvas( editorCanvas )
    {
        Modal::initTree();
    }

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;

private:
    EditorCanvasComponent & M_editorCanvas;

    std::string M_projectNameInput = "";
    std::string M_widthInput;
    std::string M_heightInput;

    ftxui::Component M_projectNameInputComponent = ftxui::Input(&M_projectNameInput, "Project Name");
    ftxui::Component M_widthInputComponent = ftxui::Input(&M_widthInput, "48");
    ftxui::Component M_heightInputComponent = ftxui::Input(&M_heightInput, "48");

};



class ResizeModal
    : public Modal
{
public:
    ResizeModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose);

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;


private:
    EditorCanvasComponent & M_editorCanvas;

    std::string M_placeholderWidth;
    std::string M_placeholderHeight;
    std::string M_widthInput;
    std::string M_heightInput;

    ftxui::Component M_widthInputComponent;
    ftxui::Component M_heightInputComponent;

};



class SaveModal
    : public Modal
{
public:
    SaveModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose, std::unordered_map<std::string, std::vector<ftxui::Color>> const& palettes = {})
        : Modal( onClose, "Save"), M_editorCanvas( editorCanvas ), M_palettes( palettes )
    {
        Modal::initTree();
    }

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;


private:
    EditorCanvasComponent & M_editorCanvas;
    std::unordered_map<std::string, std::vector<ftxui::Color>> const& M_palettes;

    std::string M_filepathInput = "";
    ftxui::Component M_filepathInputComponent = ftxui::Input(&M_filepathInput, "path/to/save.json");

};



class OpenProjectModal
    : public Modal
{
public:
    OpenProjectModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose, std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes )
        : Modal( onClose, "Open"), M_editorCanvas( editorCanvas ), M_palettes( palettes )
    {
        Modal::initTree();
    }

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;


private:
    EditorCanvasComponent & M_editorCanvas;
    std::unordered_map<std::string, std::vector<ftxui::Color>> & M_palettes;

    std::string M_filepathInput = "";
    ftxui::Component M_filepathInputComponent = ftxui::Input(&M_filepathInput, "path/to/project.json");
};




class ExportModal
    : public Modal
{
public:
    ExportModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose)
        : Modal( onClose, "Export"), M_editorCanvas( editorCanvas )
    {
        Modal::initTree();
    }

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;


private:
    EditorCanvasComponent & M_editorCanvas;

    std::string M_filepathInput = "";
    ftxui::Component M_filepathInputComponent = ftxui::Input(&M_filepathInput, "path/to/export.png");

    int M_selectedFormatIndex = 0;
    std::vector<std::string> M_formatOptions = { "png", "jpg", "bmp", "ascii" };
    ftxui::Component M_formatDropdown = ftxui::Dropdown( M_formatOptions, &M_selectedFormatIndex);
};


class ImportModal
    : public Modal
{
public:
    ImportModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose)
        : Modal( onClose, "Import"), M_editorCanvas( editorCanvas )
    {
        Modal::initTree();
    }

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;


private:
    EditorCanvasComponent & M_editorCanvas;

    std::string M_filepathInput = "";
    std::string M_targetWidthInput = "";
    std::string M_targetHeightInput = "";

    ftxui::Component M_targetWidthInputComponent = ftxui::Input(&M_targetWidthInput, "64");
    ftxui::Component M_targetHeightInputComponent = ftxui::Input(&M_targetHeightInput, "64");
    ftxui::Component M_filepathInputComponent = ftxui::Input(&M_filepathInput, "path/to/image.png");

    int M_selectedFormatIndex = 0;
    std::vector<std::string> M_formatOptions = { "png", "jpg", "bmp", "ascii" };
    ftxui::Component M_formatDropdown = ftxui::Dropdown( M_formatOptions, &M_selectedFormatIndex);
};




class BackgroundColorModal
    : public Modal
{
public:
    BackgroundColorModal( EditorState & editorState, std::function<void()> onClose)
        : Modal( onClose, "Export")
    {
        M_colorPicker = ColorPicker( editorState.backgroundColor );
        Modal::initTree();
    }

private:
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;


private:
    std::shared_ptr<ColorPickerComponent> M_colorPicker;

};



class AboutModal
    : public Modal
{
public:
    AboutModal( std::function<void()> onClose)
        : Modal( onClose, "About")
    {
        Modal::initTree();
    }

private:
    ftxui::Element renderModalContent() override;
};


class ShortcutsModal
    : public Modal
{
public:
    ShortcutsModal( ShortcutManager * shortcutManager, std::function<void()> onClose)
        : Modal( onClose, "Shortcuts"), M_shortcutManager( shortcutManager )
    {
        Modal::initTree();
    }

private:
    ftxui::Element renderModalContent() override;

private:
    ShortcutManager * M_shortcutManager;
};



class NewPaletteModal
    : public Modal
{
public:
    NewPaletteModal( std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes, std::function<void()> onClose)
        : Modal( onClose, "New Palette"), M_palettes( palettes )
    {
        Modal::initTree();
        M_paletteNameInputComponent->TakeFocus();
    }

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;

private:
    std::unordered_map<std::string, std::vector<ftxui::Color>> & M_palettes;
    std::string M_paletteNameInput = "";
    ftxui::Component M_paletteNameInputComponent = ftxui::Input(&M_paletteNameInput, "Palette Name");
};



class ImportPaletteModal
    : public Modal
{
public:
    ImportPaletteModal( std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes, std::function<void()> onClose )
        : Modal( onClose, "Import Palette"), M_palettes( palettes )
    {
        Modal::initTree();
        M_paletteNameInputComponent->TakeFocus();
    }

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;

private:
    //TODO: Maybe use sort of back inserter here instead of map
    std::unordered_map<std::string, std::vector<ftxui::Color>> & M_palettes;

    std::string M_paletteNameInput = "";
    ftxui::Component M_paletteNameInputComponent = ftxui::Input(&M_paletteNameInput, "Palette Name");

    std::string M_paletteFilepathInput = "";
    ftxui::Component M_paletteFilepathInputComponent = ftxui::Input(&M_paletteFilepathInput, "path/to/palette.gpl");

    std::vector<std::string> M_formatOptions = { "gpl", "png" };
    int M_selectedFormatIndex = 0;
    ftxui::Component M_formatDropdown = ftxui::Dropdown( M_formatOptions, &M_selectedFormatIndex);

};



class ExportPaletteModal
    : public Modal
{
public:
    ExportPaletteModal( std::function<void()> onClose )
        : Modal( onClose, "Export Palette")
    {
        Modal::initTree();
        M_paletteFilepathInputComponent->TakeFocus();
    }

    void setPalette( std::vector<ftxui::Color> const& palette, std::string const& paletteName )
    {
        M_palette = palette;
        M_paletteName = paletteName;
    }

private:
    void onConfirm() override;
    ftxui::Component buildContentComponent() const override;
    ftxui::Element renderModalContent() override;

private:
    std::vector<ftxui::Color> M_palette;

    std::string M_paletteName;

    std::string M_paletteFilepathInput = "";
    ftxui::Component M_paletteFilepathInputComponent = ftxui::Input(&M_paletteFilepathInput, "path/to/palette");

    std::vector<std::string> M_formatOptions = { "gpl", "png" };
    int M_selectedFormatIndex = 0;
    ftxui::Component M_formatDropdown = ftxui::Dropdown( M_formatOptions, &M_selectedFormatIndex);

};


}
