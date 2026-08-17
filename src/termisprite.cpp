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

    M_brushSettings = BrushSettings( M_editorCanvas->currentState(), &M_shortcutManager );
    M_eraserSettings = EraserSettings( M_editorCanvas->currentState(), &M_shortcutManager );
    M_rectangleSettings = RectangleSettings( M_editorCanvas->currentState(), &M_shortcutManager );
    M_ellipseSettings = EllipseSettings( M_editorCanvas->currentState(), &M_shortcutManager );
    M_lineSettings = LineSettings( M_editorCanvas->currentState(), &M_shortcutManager );
    M_paintFillSettings = PaintFillSettings( M_editorCanvas->currentState(), &M_shortcutManager );
    M_boxSelectSettings = BoxSelectSettings( M_editorCanvas->currentState(), &M_shortcutManager );

    M_newProjectModal = std::make_shared<NewProjectModal>( *M_editorCanvas, [this]{ M_showNewProjectModal = false; });
    M_openProjectModal = std::make_shared<OpenProjectModal>( *M_editorCanvas, [this]{ M_showOpenProjectModal = false; });
    M_saveModal = std::make_shared<SaveModal>( *M_editorCanvas, [this]{ M_showSaveModal = false; });
    M_exportModal = std::make_shared<ExportModal>( *M_editorCanvas, [this]{ M_showExportModal = false; });
    M_resizeModal = std::make_shared<ResizeModal>( *M_editorCanvas, [this]{ M_showResizeModal = false; });
    M_aboutModal = std::make_shared<AboutModal>( [this]{ M_showAboutModal = false; });
    M_shortcutsModal = std::make_shared<ShortcutsModal>(&M_shortcutManager, [this]{ M_showShortcutsModal = false; });

    M_editorCanvas->onBackgroundChangeRequested = [this] { M_showBackgroundColorModal = true; };
    M_backgroundColorModal = std::make_shared<BackgroundColorModal>( M_editorCanvas->currentState(), [this]{ M_showBackgroundColorModal = false; });

    M_importModal = std::make_shared<ImportModal>( *M_editorCanvas, [this]{ M_showImportModal = false; });


    M_settingsContainer = ftxui::Container::Vertical({
        ftxui::Maybe( M_brushSettings, [this] { return M_editorCanvas->currentState().toolType == ToolType::DRAW; }),
        ftxui::Maybe( M_eraserSettings, [this] { return M_editorCanvas->currentState().toolType == ToolType::ERASER; }),
        ftxui::Maybe( M_rectangleSettings, [this] { return M_editorCanvas->currentState().toolType == ToolType::SQUARE; }),
        ftxui::Maybe( M_ellipseSettings, [this] { return M_editorCanvas->currentState().toolType == ToolType::CIRCLE; }),
        ftxui::Maybe( M_lineSettings, [this] { return M_editorCanvas->currentState().toolType == ToolType::LINE; }),
        ftxui::Maybe( M_paintFillSettings, [this] { return M_editorCanvas->currentState().toolType == ToolType::PAINT_FILL; }),
        ftxui::Maybe( M_boxSelectSettings, [this] { return M_editorCanvas->currentState().toolType == ToolType::BOX_SELECT; })

    });

    ftxui::Component baseContainer = ftxui::Container::Vertical({
        ftxui::Container::Horizontal({
            M_menu, M_editorCanvas, M_settingsContainer, M_tools, M_colorSection
        }),
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
                        M_settingsContainer->Render(),
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
    if ( ftxui::ComponentBase::OnEvent( event ) )
        return true;

    if ( M_shortcutManager.handleEvent( event ) )
        return true;


    return false;

}



}
