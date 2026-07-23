#include <ftxui/dom/elements.hpp>
#include "menu.hpp"

namespace Termisprite
{

MenuComponent::MenuComponent( std::vector<MenuCategory> categories )
    : M_categories( std::move(categories) )
{
    M_selectedIndices.resize( M_categories.size(), 0 );
    M_buttonBoxes.resize( M_categories.size() );

    ftxui::Components buttons;
    for ( size_t i = 0; i < M_categories.size(); ++i )
    {
        buttons.push_back( ftxui::Button( M_categories[i].name, [this, i]() {
            if ( M_activeDropdown == static_cast<int>(i) ) M_activeDropdown = -1;
            else M_activeDropdown = static_cast<int>(i);
        }, ftxui::ButtonOption::Ascii() ) );

        M_dropdownMenus.push_back( ftxui::Menu( &M_categories[i].options, &M_selectedIndices[i] ) );
    }

    M_buttonsContainer = ftxui::Container::Horizontal( std::move(buttons) );
    Add( M_buttonsContainer );
}

ftxui::Element MenuComponent::OnRender()
{
    ftxui::Elements buttonElements;
    for ( size_t i = 0; i < M_categories.size(); ++i )
    {
        auto & btn = M_buttonsContainer->ChildAt(i);
        auto btnElt = btn->Render() | ftxui::reflect(M_buttonBoxes[i]);
        if ( btn->Focused() )
            btnElt |= ftxui::color( ftxui::Color::Cyan );

        buttonElements.push_back( btnElt );
    }
    return ftxui::hbox( std::move(buttonElements) ) | ftxui::xflex;
}

ftxui::Element MenuComponent::RenderOverlay()
{
    if ( M_activeDropdown == -1 )
        return ftxui::text("");

    ftxui::Element dropdown = ftxui::window( ftxui::text(""), M_dropdownMenus[M_activeDropdown]->Render() )
                            | ftxui::clear_under
                            | ftxui::reflect( M_dropdownBox );

    int offset_x = M_buttonBoxes[M_activeDropdown].x_min;

    return ftxui::vbox({
        ftxui::text("") | ftxui::size( ftxui::HEIGHT, ftxui::EQUAL, 1 ),
        ftxui::hbox({
            ftxui::text("") | ftxui::size( ftxui::WIDTH, ftxui::EQUAL, offset_x ),
            dropdown
        })
    });
}

bool MenuComponent::OnEvent( ftxui::Event event )
{
    if (event == ftxui::Event::Tab || event == ftxui::Event::TabReverse)
        return false;

    if ( M_activeDropdown != -1 )
    {
        if ( event == ftxui::Event::Escape )
        {
            M_activeDropdown = -1;
            return true;
        }

        if ( event.is_mouse() )
        {
            auto mouse = event.mouse();
            if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed )
            {
                if ( !M_dropdownBox.Contain( mouse.x, mouse.y ) && !M_buttonBoxes[M_activeDropdown].Contain( mouse.x, mouse.y ) )
                {
                    bool clickedOtherButton = false;
                    for ( size_t i = 0; i < M_buttonBoxes.size(); ++i )
                    {
                        if ( static_cast<int>(i) != M_activeDropdown && M_buttonBoxes[i].Contain( mouse.x, mouse.y ) )
                        {
                            M_activeDropdown = static_cast<int>(i);
                            clickedOtherButton = true;
                            break;
                        }
                    }

                    if ( !clickedOtherButton )
                    {
                        M_activeDropdown = -1;
                        return false;
                    }
                    else
                        return true;
                }
            }
        }

        bool handled = M_dropdownMenus[M_activeDropdown]->OnEvent( event );

        bool isEnter = ( event == ftxui::Event::Return || event == ftxui::Event::Character('\n') );
        bool isClick = ( event.is_mouse() &&
                          event.mouse().button == ftxui::Mouse::Button::Left &&
                          event.mouse().motion == ftxui::Mouse::Pressed &&
                          M_dropdownBox.Contain( event.mouse().x, event.mouse().y ) );

        if ( handled && ( isEnter || isClick ) )
        {
            int selected = M_selectedIndices[M_activeDropdown];
            auto callback = M_categories[M_activeDropdown].onSelect;

            M_activeDropdown = -1;

            if ( callback )
                callback( selected );
            return true;
        }

        if ( handled ) return true;
    }

    return M_buttonsContainer->OnEvent( event );
}

std::shared_ptr<MenuComponent> Menu( std::vector<MenuCategory> categories )
{
    return std::make_shared<MenuComponent>( std::move(categories) );
}

}
