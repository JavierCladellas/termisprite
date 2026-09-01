

#include "layers.hpp"
#include <ftxui/component/component.hpp>

namespace Termisprite
{



LayersComponent::LayersComponent( EditorCanvasComponent * editor, ShortcutManager * shortcutManager )
    : M_editor( editor), M_shortcutManager( shortcutManager )
{
    rebuild();
}

ftxui::Element
LayersComponent::OnRender()
{
    using namespace ftxui;

    Color borderColor = Focused() ? Color::Cyan : Color::White;

    return window( text(" Layers ") | bold | center,
        ComponentBase::OnRender()
    ) | color( borderColor );

}

bool
LayersComponent::OnEvent( ftxui::Event event )
{
    if ( Focused() && ( event == ftxui::Event::Tab ) || ( event == ftxui::Event::TabReverse ) )
        return false;


    return ComponentBase::OnEvent( event );
}


ftxui::Component
LayersComponent::createLayerRow( int index )
{
    using namespace ftxui;

    auto & layer = M_editor->layers()[index];

    auto visibilityBtn = ftxui::Button(
        layer->isVisible() ? "O" : " ",
        [this, &layer, index](){ layer->toggleVisibility(); M_editor->saveState(); rebuild();},
        ftxui::ButtonOption::Ascii()
    );

    auto selectBtn = ftxui::Button(
        layer->name(),
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

    auto rowContainer = Container::Horizontal({  selectBtn, visibilityBtn, moveUpBtn, moveDownBtn, deleteBtn });

    return Renderer(rowContainer, [this, index, visibilityBtn, selectBtn, moveUpBtn, moveDownBtn,deleteBtn](){
        return hbox({
            selectBtn->Render() | flex | ( index == M_editor->activeLayerIndex() ? ( bold | color(Color::Green) ) : ( color(Color::White) | dim  ) ),
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


