#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

#include "termisprite.hpp"
#include "editor.hpp"


namespace Termisprite
{


Termisprite::Termisprite()
    : M_shortcutManager( this )
{
    M_editorCanvas = EditorCanvas( 32, 32 );

    M_menu = Menu( &M_shortcutManager );
    M_tools = ToolsSection( M_editorCanvas->currentState(), &M_shortcutManager );
    M_colorSection = ColorSection( M_editorCanvas->currentState() );
    M_statusBar = StatusBar( M_editorCanvas->currentState(), &M_shortcutManager );

    M_newProjectModal = std::make_shared<NewProjectModal>( *M_editorCanvas, [this]{ M_showNewProjectModal = false; });
    M_openProjectModal = std::make_shared<OpenProjectModal>( *M_editorCanvas, [this]{ M_showOpenProjectModal = false; });
    M_saveModal = std::make_shared<SaveModal>( *M_editorCanvas, [this]{ M_showSaveModal = false; });
    M_exportModal = std::make_shared<ExportModal>( *M_editorCanvas, [this]{ M_showExportModal = false; });
    M_resizeModal = std::make_shared<ResizeModal>( *M_editorCanvas, [this]{ M_showResizeModal = false; });
    M_aboutModal = std::make_shared<AboutModal>( [this]{ M_showAboutModal = false; });
    M_shortcutsModal = std::make_shared<ShortcutsModal>( [this]{ M_showShortcutsModal = false; });

    M_editorCanvas->onBackgroundChangeRequested = [this] { M_showBackgroundColorModal = true; };
    M_backgroundColorModal = std::make_shared<BackgroundColorModal>( M_editorCanvas->currentState(), [this]{ M_showBackgroundColorModal = false; });

    M_importModal = std::make_shared<ImportModal>( *M_editorCanvas, [this]{ M_showImportModal = false; });

    ftxui::Component baseContainer = ftxui::Container::Vertical({
        ftxui::Container::Horizontal({ M_menu, M_editorCanvas,  M_tools, M_colorSection }),
        M_statusBar
    });

    M_masterComponent = ftxui::Renderer(baseContainer, [this] {
        return ftxui::dbox({
            ftxui::vbox({
                M_menu->Render(),
                ftxui::separatorEmpty(),
                ftxui::hbox({
                    M_editorCanvas->Render() | ftxui::flex,
                    ftxui::vbox({
                        M_tools->Render(),
                        M_colorSection->Render()
                    })
                }) | ftxui::flex,
                ftxui::filler(),
                M_statusBar->Render()
            }),
            M_menu->RenderOverlay()
        });
    });

    M_masterComponent |= ftxui::Modal(M_newProjectModal, &M_showNewProjectModal);
    M_masterComponent |= ftxui::Modal(M_saveModal, &M_showSaveModal);
    M_masterComponent |= ftxui::Modal(M_exportModal, &M_showExportModal);
    M_masterComponent |= ftxui::Modal(M_openProjectModal, &M_showOpenProjectModal);
    M_masterComponent |= ftxui::Modal(M_resizeModal, &M_showResizeModal);
    M_masterComponent |= ftxui::Modal(M_aboutModal, &M_showAboutModal);
    M_masterComponent |= ftxui::Modal(M_shortcutsModal, &M_showShortcutsModal);
    M_masterComponent |= ftxui::Modal(M_backgroundColorModal, &M_showBackgroundColorModal);
    M_masterComponent |= ftxui::Modal(M_importModal, &M_showImportModal);

    ftxui::ComponentBase::Add(M_masterComponent);
}


ftxui::Element
Termisprite::OnRender()
{
    return M_masterComponent->Render();
}


bool
Termisprite::OnEvent( ftxui::Event event )
{
    if ( M_shortcutManager.handleEvent( event ) )
        return true;

    if ( ftxui::ComponentBase::OnEvent( event ) )
        return true;

    return false;

}



}
