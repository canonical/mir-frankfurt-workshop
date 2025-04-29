// mirly_window_manager.cpp
#define MIR_LOG_COMPONENT "mirly-window-manager"

#include "mirly_window_manager.h"

#include <mir/log.h>
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

auto MirlyWindowManager::place_new_window(
    miral::ApplicationInfo const& app_info,
    miral::WindowSpecification const& requested_specification) -> miral::WindowSpecification
{
    if (requested_specification.type() != mir_window_type_normal && requested_specification.type() != mir_window_type_freestyle)
        return requested_specification;

    auto const placement_rectangle = tools.active_output();
    int const x = placement_rectangle.top_left.x.as_value();
    int const y = placement_rectangle.top_left.y.as_value();
    int const half_width = static_cast<float>(placement_rectangle.size.width.as_int()) / 2.f;
    int const half_height = static_cast<float>(placement_rectangle.size.height.as_int()) / 2.f;
    mir::geometry::Rectangle placement;
    switch (window_count)
    {
    case 0: placement = mir::geometry::Rectangle{{x, y}, {half_width, half_height}}; break;
    case 1: placement = mir::geometry::Rectangle{{x + half_width, y}, {half_width, half_height}}; break;
    case 2: placement = mir::geometry::Rectangle{{x, y + half_height}, {half_width, half_height}}; break;
    case 3: placement = mir::geometry::Rectangle{{x + half_width, y + half_height}, {half_width, half_height}}; break;
    default:
        mir::log_error("Logic error: %d is greater than 3", window_count);
        break;
    }

    window_count++;
    if (window_count > 3)
        window_count = 0;

    miral::WindowSpecification specification = requested_specification;
    specification.top_left() = placement.top_left;
    specification.size() = placement.size;
    return specification;
}
