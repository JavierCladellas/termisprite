#pragma once

#include "colorpicker.hpp"
#include "editor.hpp"
#include "menu.hpp"
#include "statusbar.hpp"
#include "tools.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <functional>


namespace Termisprite
{

class NewProjectModal
    : public ftxui::ComponentBase
{
public:
    NewProjectModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose);

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:
    EditorCanvasComponent & M_editorCanvas;
    std::function<void()> M_closeCallback;

    std::string M_projectNameInput = "";
    std::string M_widthInput;
    std::string M_heightInput;

    ftxui::Component M_projectNameInputComponent = ftxui::Input(&M_projectNameInput, "Project Name");
    ftxui::Component M_widthInputComponent = ftxui::Input(&M_widthInput, "32");
    ftxui::Component M_heightInputComponent = ftxui::Input(&M_heightInput, "32");

    ftxui::Component M_okButton = ftxui::Button("OK", [this] {
        if ( M_widthInput.empty() )
            M_widthInput = "32";
        if ( M_heightInput.empty() )
            M_heightInput = "32";
        M_editorCanvas.clear();
        M_editorCanvas.resize(std::stoi(M_widthInput), std::stoi(M_heightInput));
        M_projectNameInput = "";
        M_widthInput = "";
        M_heightInput = "";
        M_closeCallback();
    });
    ftxui::Component M_cancelButton = ftxui::Button("Cancel", [this] { M_closeCallback(); });

    ftxui::Box M_box;
};


//TODO: Refactor into a single component
class ResizeModal
    : public ftxui::ComponentBase
{
public:
    ResizeModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose);

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:
    EditorCanvasComponent & M_editorCanvas;
    std::function<void()> M_closeCallback;

    std::string M_placeholderWidth;
    std::string M_placeholderHeight;
    std::string M_widthInput;
    std::string M_heightInput;

    ftxui::Component M_widthInputComponent;
    ftxui::Component M_heightInputComponent;

    ftxui::Component M_okButton;
    ftxui::Component M_cancelButton = ftxui::Button("Cancel", [this] { M_closeCallback(); });

    ftxui::Box M_box;

};


class SaveModal
    : public ftxui::ComponentBase
{
public:
    SaveModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose);

    ftxui::Element OnRender() override;

private:
    EditorCanvasComponent & M_editorCanvas;
    std::function<void()> M_closeCallback;

    std::string M_filepathInput = "";
    ftxui::Component M_filepathInputComponent = ftxui::Input(&M_filepathInput, "path/to/save.json");

    ftxui::Component M_okButton;
    ftxui::Component M_cancelButton = ftxui::Button("Cancel", [this] { M_closeCallback(); });

    ftxui::Box M_box;
};


class ExportModal
    : public ftxui::ComponentBase
{
public:
    ExportModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose);

    ftxui::Element OnRender() override;

private:
    EditorCanvasComponent & M_editorCanvas;
    std::function<void()> M_closeCallback;

    std::string M_filepathInput = "";
    ftxui::Component M_filepathInputComponent = ftxui::Input(&M_filepathInput, "path/to/export.png");

    int M_selectedFormatIndex = 0;
    std::vector<std::string> M_formatOptions = { "png" };
    ftxui::Component M_formatDropdown = ftxui::Dropdown( M_formatOptions, &M_selectedFormatIndex);

    ftxui::Component M_okButton;
    ftxui::Component M_cancelButton = ftxui::Button("Cancel", [this] { M_closeCallback(); });

    ftxui::Box M_box;
};

class OpenProjectModal
    : public ftxui::ComponentBase
{
public:
    OpenProjectModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose );

    ftxui::Element OnRender() override;

private:
    EditorCanvasComponent & M_editorCanvas;
    std::function<void()> M_closeCallback;

    std::string M_filepathInput = "";
    ftxui::Component M_filepathInputComponent = ftxui::Input(&M_filepathInput, "path/to/project.json");

    ftxui::Component M_okButton;
    ftxui::Component M_cancelButton = ftxui::Button("Cancel", [this] { M_closeCallback(); });

    ftxui::Box M_box;
};

class AboutModal
    : public ftxui::ComponentBase
{
public:
    AboutModal( std::function<void()> onClose );

    ftxui::Element OnRender() override;

private:
    std::function<void()> M_closeCallback;
    ftxui::Component M_closeButton;
};

class ShortcutsModal
    : public ftxui::ComponentBase
{
public:
    ShortcutsModal( std::function<void()> onClose );

    ftxui::Element OnRender() override;

private:
    std::function<void()> M_closeCallback;
    ftxui::Component M_closeButton;
};

class BackgroundColorModal
    : public ftxui::ComponentBase
{
public:
    BackgroundColorModal( EditorState & editorState, std::function<void()> onClose );
    ftxui::Element OnRender() override;

private:
    std::function<void()> M_closeCallback;
    std::shared_ptr<ColorPickerComponent> M_colorPicker;
    ftxui::Component M_closeButton;
};

class ImportModal
    : public ftxui::ComponentBase
{
public:
    ImportModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose);

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:
    EditorCanvasComponent & M_editorCanvas;
    std::function<void()> M_closeCallback;

    std::string M_filepathInput = "";
    std::string M_targetWidthInput = "";
    std::string M_targetHeightInput = "";

    ftxui::Component M_targetWidthInputComponent = ftxui::Input(&M_targetWidthInput, "64");
    ftxui::Component M_targetHeightInputComponent = ftxui::Input(&M_targetHeightInput, "64");
    ftxui::Component M_filepathInputComponent = ftxui::Input(&M_filepathInput, "path/to/image.png");

    ftxui::Component M_okButton;
    ftxui::Component M_cancelButton;
    ftxui::Box M_box;
};


class Termisprite
    : public ftxui::ComponentBase
{
public:
    Termisprite();

    ftxui::Element OnRender() override;

    bool OnEvent( ftxui::Event event ) override;

private:

    bool processHistoryEvents( ftxui::Event event );
    bool processClipboardEvents( ftxui::Event event );
    bool processToggleGrid( ftxui::Event event );
    bool processClearSelection( ftxui::Event event );
    bool processToolSelection( ftxui::Event event );


private:
    ftxui::Component M_masterComponent;

    std::shared_ptr<EditorCanvasComponent> M_editorCanvas;
    std::shared_ptr<MenuComponent> M_menu;
    std::shared_ptr<ToolsComponent> M_tools;
    std::shared_ptr<ColorSectionComponent> M_colorSection;
    std::shared_ptr<StatusBarComponent> M_statusBar;

    bool M_showNewProjectModal = false;
    std::shared_ptr<NewProjectModal> M_newProjectModal;

    bool M_showSaveModal = false;
    std::shared_ptr<SaveModal> M_saveModal;

    bool M_showExportModal = false;
    std::shared_ptr<ExportModal> M_exportModal;

    bool M_showResizeModal = false;
    std::shared_ptr<ResizeModal> M_resizeModal;

    bool M_showAboutModal = false;
    std::shared_ptr<AboutModal> M_aboutModal;

    bool M_showShortcutsModal = false;
    std::shared_ptr<ShortcutsModal> M_shortcutsModal;

    bool M_showBackgroundColorModal = false;
    std::shared_ptr<BackgroundColorModal> M_backgroundColorModal;

    bool M_showImportModal = false;
    std::shared_ptr<ImportModal> M_importModal;

    bool M_showOpenProjectModal = false;
    std::shared_ptr<OpenProjectModal> M_openProjectModal;

};


}
