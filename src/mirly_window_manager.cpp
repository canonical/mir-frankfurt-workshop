// mirly_window_manager.cpp
#include "mirly_window_manager.h"

#include <miral/toolkit_event.h>
#include <xkbcommon/xkbcommon-keysyms.h>
MirlyWindowManager::MirlyWindowManager(miral::WindowManagerTools const& tools)
    : miral::MinimalWindowManager(tools)
{
}

bool MirlyWindowManager::handle_keyboard_event(MirKeyboardEvent const* event)
{
    if (miral::toolkit::mir_keyboard_event_action(event) != mir_keyboard_action_down)
        return false;

    MirInputEventModifiers mods = miral::toolkit::mir_keyboard_event_modifiers(event);
    if (!(mods & mir_input_event_modifier_alt) || !(mods & mir_input_event_modifier_ctrl))
        return false;

    switch (miral::toolkit::mir_keyboard_event_keysym(event))
    {
        case XKB_KEY_q:
        case XKB_KEY_Q:
            tools.ask_client_to_close(tools.active_window());
            return true;
        default:
            return false;
    }
}
