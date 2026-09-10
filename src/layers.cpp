

#include "layers.hpp"
#include "sprite.hpp"
#include "sprite_io.hpp"
#include <ftxui/component/component.hpp>
#include <memory>

namespace Termisprite
{

ftxui::Element
LayerContextMenuComponent::OnRender()
{
    ftxui::Element modal = ftxui::window( ftxui::text(" Layer Options "), M_menu->Render() )
                         | ftxui::clear_under
                         | ftxui::reflect( M_box );

    ftxui::Element positionedModal = ftxui::vbox({
        ftxui::text("") | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, M_posY),
        ftxui::hbox({
            ftxui::text("") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, M_posX),
            modal
        })
    });

    return positionedModal;
}

bool
LayerContextMenuComponent::OnEvent( ftxui::Event event )
{
    if ( !M_isActive )
        return false;

    bool handled = M_menu->OnEvent( event );

    bool executeAction = ( event == ftxui::Event::Return || event == ftxui::Event::Character('\n') );

    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();
        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed )
        {
            if ( M_box.Contain( mouse.x, mouse.y ) )
                executeAction = true;
            else
            {
                M_isActive = false;
                return true;
            }
        }
        if ( mouse.button == ftxui::Mouse::Button::Right && mouse.motion == ftxui::Mouse::Pressed )
        {
            if ( !M_box.Contain( mouse.x, mouse.y ) )
            {
                hide();
            }
        }
    }

    if ( executeAction )
    {
        switch ( M_menuIndex )
        {
            case 0:
                if ( onRenameAction )
                    onRenameAction(M_layerIndex);
                break;
            case 1: M_editor->duplicateLayer(M_layerIndex,true); break;
            case 2: M_editor->removeLayer(M_layerIndex); break;
            case 3: M_editor->layers()[M_layerIndex]->clear(); break;
            case 4: M_editor->layers()[M_layerIndex]->toggleVisibility(); break;
            case 5:
            {
                auto const& layer = M_editor->layers()[M_layerIndex];
                std::vector<std::unique_ptr<Layer>> layersToExport;
                layersToExport.push_back(std::make_unique<Layer>(*layer));
                SpriteExporter::exportImage(layer->name()+".png", layersToExport);
                break;
            }
            case 6: M_editor->moveLayerUp(M_layerIndex); break;
            case 7: M_editor->moveLayerDown(M_layerIndex); break;
            default: break;
        }
        M_isActive = false;
        M_menuIndex = 0;
        return true;
    }

    if ( event == ftxui::Event::Escape )
    {
        M_isActive = false;
        return true;
    }

    return true;

}

std::shared_ptr<LayerContextMenuComponent>
LayerContextMenu( EditorCanvasComponent * editor )
{
    return std::make_shared<LayerContextMenuComponent>( editor );
}



LayersComponent::LayersComponent( EditorCanvasComponent * editor, ShortcutManager * shortcutManager )
    : M_editor( editor), M_shortcutManager( shortcutManager ), M_contextMenu( LayerContextMenu(editor) )
{

    M_renameInput = ftxui::Input(&M_renameBuffer, "Layer Name", {
        .multiline = false,
        .on_enter = [this]() {
            std::string trimmedName = M_renameBuffer;
            if ( trimmedName.ends_with("\n") )
                trimmedName.pop_back();
            M_editor->layers()[M_renamingLayerIndex]->setName(trimmedName);
            M_renamingLayerIndex = -1;
            rebuild();
        }
    });

    M_renameInput |= ftxui::CatchEvent([this](ftxui::Event event) {
        if (event == ftxui::Event::Escape )
        {
            M_renamingLayerIndex = -1;
            rebuild();
            return true;
        }
        return false;
    });

    M_contextMenu->onRenameAction = [this](int index) {
        M_renamingLayerIndex = index;
        M_renameBuffer = M_editor->layers()[index]->name();
        rebuild();
        M_renameInput->TakeFocus();
    };

    M_editor->onLayersChanged = [this]() { rebuild(); };
    rebuild();
}

ftxui::Element
LayersComponent::OnRender()
{
    using namespace ftxui;

    Color borderColor = Focused() ? Color::Cyan : Color::White;

    auto windowContent = ComponentBase::OnRender() | reflect(M_box);

    if ( M_contextMenu->isActive() )
        windowContent = dbox({ windowContent, M_contextMenu->Render() });

    return window( text(" Layers ") | bold | center,
        windowContent
    )  | color( borderColor );


}

bool
LayersComponent::OnEvent( ftxui::Event event )
{
    if ( M_contextMenu->isActive() )
        if ( M_contextMenu->OnEvent( event ) )
        {
            rebuild();
            return true;
        }


    if ( Focused() && ( event == ftxui::Event::Tab ) || ( event == ftxui::Event::TabReverse ) )
        return false;

    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();
        if ( mouse.button == ftxui::Mouse::Button::Right && mouse.motion == ftxui::Mouse::Pressed )
        {
            if ( M_box.Contain( mouse.x, mouse.y ) )
            {

                int clickedRowIndex = mouse.y - M_box.y_min;
                if ( clickedRowIndex >= 0 && clickedRowIndex < static_cast<int>(M_editor->layers().size()) )
                {
                    M_contextMenu->setLayerIndex(clickedRowIndex);

                    M_contextMenu->x() = std::clamp(mouse.x - M_box.x_min, 0, M_box.x_max - M_box.x_min);
                    M_contextMenu->y() = std::clamp(mouse.y - M_box.y_min, 0, M_box.y_max - M_box.y_min);

                    M_contextMenu->show();
                }

                return true;
            }
        }
    }

    return ComponentBase::OnEvent( event );
}


ftxui::Component
LayersComponent::createLayerRow( int index )
{
    using namespace ftxui;

    auto & layer = M_editor->layers()[index];

    auto visibilityBtn = ftxui::Button(
        layer->isVisible() ? "O" : " ",
        [this, index](){ M_editor->layers()[index]->toggleVisibility(); M_editor->saveState(); rebuild();},
        ftxui::ButtonOption::Ascii()
    );

    ftxui::Component nameBtn;
    if ( M_renamingLayerIndex == index )
        nameBtn = M_renameInput;
    else
        nameBtn = ftxui::Button( layer->name(),
            [this, index](){ M_editor->setActiveLayer(index); },
            ftxui::ButtonOption::Ascii()
        );

    auto moveUpBtn = ftxui::Button(
        "↑",
        [this, index](){ M_editor->moveLayerUp(index); rebuild();},
        ftxui::ButtonOption::Ascii()
    );
    auto moveDownBtn = ftxui::Button(
        "↓",
        [this, index](){ M_editor->moveLayerDown(index); rebuild();},
        ftxui::ButtonOption::Ascii()
    );
    auto deleteBtn = ftxui::Button(
        "x",
        [this, index](){ M_editor->removeLayer(index); rebuild();},
        ftxui::ButtonOption::Ascii()
    );

    auto rowContainer = Container::Horizontal({  nameBtn, visibilityBtn, moveUpBtn, moveDownBtn, deleteBtn });

    return Renderer(rowContainer, [this, index, visibilityBtn, nameBtn, moveUpBtn, moveDownBtn,deleteBtn](){
        auto nameElt = nameBtn->Render() | flex;
        if (M_renamingLayerIndex != index)
            nameElt |=  ( index == M_editor->activeLayerIndex() ? ( bold | color(Color::Green) ) : ( color(Color::White) | dim ) );

        return hbox({
                nameElt,
            text(" "),
            visibilityBtn->Render(),
            moveUpBtn->Render(),
            moveDownBtn->Render(),
            deleteBtn->Render() | color(Color::Red)
        });
    });

}


void
LayersComponent::rebuild()
{
    DetachAllChildren();

    ftxui::Components layerRows;
    for (int i = 0; i < static_cast<int>(M_editor->layers().size()); ++i)
        layerRows.push_back(createLayerRow(i));

    auto addButton = Button("+ Add Layer", [this] {
        M_editor->addLayer("Layer " + std::to_string(M_editor->layers().size() + 1));
        M_editor->setActiveLayer(0);
        rebuild();
    }, ftxui::ButtonOption::Ascii());

    auto toggleActiveBorderCheckbox = ftxui::Checkbox("Show Boundaries", &M_editor->activeLayerBorderVisible());

    auto layout = ftxui::Container::Vertical(std::move(layerRows));
    layout->Add(ftxui::Renderer([]{ return ftxui::separatorEmpty(); }));
    layout->Add(addButton);
    layout->Add(ftxui::Renderer([]{ return ftxui::separator(); }));
    layout->Add(toggleActiveBorderCheckbox);

    Add(layout);
}



std::shared_ptr<LayersComponent>
LayersSection( EditorCanvasComponent * editor, ShortcutManager * shortcutManager )
{
    return std::make_shared<LayersComponent>( editor, shortcutManager );
}



}


