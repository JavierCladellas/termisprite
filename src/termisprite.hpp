#pragma once

#include "colorpicker.hpp"
#include "editor.hpp"
#include "menu.hpp"
#include "shortcuts.hpp"
#include "statusbar.hpp"
#include "tools.hpp"
#include "modals.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>


namespace Termisprite
{




class Termisprite
    : public ftxui::ComponentBase
{
public:
    Termisprite();

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;


    void showNewProjectModal() { M_showNewProjectModal = true; }
    void showSaveModal() { M_showSaveModal = true; }
    void showExportModal() { M_showExportModal = true; }
    void showResizeModal() { M_showResizeModal = true; }
    void showAboutModal() { M_showAboutModal = true; }
    void showShortcutsModal() { M_showShortcutsModal = true; }
    void showBackgroundColorModal() { M_showBackgroundColorModal = true; }
    void showImportModal() { M_showImportModal = true; }
    void showOpenProjectModal() { M_showOpenProjectModal = true; }

    EditorCanvasComponent * editor() { return M_editorCanvas.get(); }
    void selectTool(ToolType type) { M_tools->selectTool(type); }


private:
    ftxui::Component M_masterComponent;

    std::shared_ptr<EditorCanvasComponent> M_editorCanvas;
    std::shared_ptr<MenuComponent> M_menu;
    std::shared_ptr<ToolsComponent> M_tools;
    std::shared_ptr<ColorSectionComponent> M_colorSection;
    std::shared_ptr<StatusBarComponent> M_statusBar;


    //TODO Maybe use a struct for this and array
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


    ShortcutManager M_shortcutManager;
};


}
