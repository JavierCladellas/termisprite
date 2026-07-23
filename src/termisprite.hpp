#pragma once

#include "colorpicker.hpp"
#include "editor.hpp"
#include "menu.hpp"
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

};


}
