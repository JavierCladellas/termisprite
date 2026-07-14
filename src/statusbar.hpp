#pragma once

#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>


namespace Termisprite
{


class StatusBarComponent
    : public ftxui::ComponentBase
{
public:
    StatusBarComponent()
    {
    }

    ftxui::Element OnRender() override
    {
        return ftxui::text( "Status Bar" )  | ftxui::border;
    }

    // bool OnEvent( ftxui::Event event ) override
    // {
    // }

private:

};


ftxui::Component StatusBar()
{
    return std::make_shared<StatusBarComponent>();
}


}
