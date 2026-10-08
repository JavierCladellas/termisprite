#include "contextwindow.hpp"

namespace Termisprite
{

ftxui::Element
ContextWindowComponent::OnRender()
{
    ftxui::Element modal = ftxui::window( ftxui::text(" Options "), M_rightClickModal->Render() )
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
ContextWindowComponent::OnEvent( ftxui::Event event )
{
    if ( !M_isActive )
        return false;

    bool handled = M_rightClickModal->OnEvent( event );

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
        switch ( M_rightClickModalIndex )
        {
            case 0: M_shortcutManager->execute( ShortcutType::BACKGROUND_COLOR ); break;
            case 1: M_shortcutManager->execute( ShortcutType::TOGGLE_GRID ); break;
            case 2: M_shortcutManager->execute( ShortcutType::CHANGE_GRID_TYPE ); break;
            case 3: M_shortcutManager->execute( ShortcutType::UNDO ); break;
            case 4: M_shortcutManager->execute( ShortcutType::REDO ); break;
            case 5: M_shortcutManager->execute( ShortcutType::CLEAR ); break;
            default: break;
        }
        M_isActive = false;
        M_rightClickModalIndex = 0;
        return true;
    }

    if ( event == ftxui::Event::Escape )
    {
        M_isActive = false;
        return true;
    }

    return true;

}


std::shared_ptr<ContextWindowComponent>
ContextWindow( ShortcutManager * shortcutManager )
{
    return std::make_shared<ContextWindowComponent>( shortcutManager );
}

}
