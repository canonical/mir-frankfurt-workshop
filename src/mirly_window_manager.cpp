// mirly_window_manager.cpp
#define MIR_LOG_COMPONENT "mirly-window-manager"

#include "mirly_window_manager.h"

#include <mir/log.h>
#include <miral/toolkit_event.h>
#include <xkbcommon/xkbcommon-keysyms.h>

namespace
{
class ExtraWindowData
{
public:
    ExtraWindowData(mir::geometry::Rectangle const& placement)
        : original_placement(placement) {}
    mir::geometry::Rectangle const original_placement;
    bool is_centered = false;
};
}

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
        //
        case XKB_KEY_q:
        case XKB_KEY_Q:
            tools.ask_client_to_close(tools.active_window());
            return true;
        case XKB_KEY_c:
        case XKB_KEY_C:
        {
            auto const active_window = tools.active_window();
            if (!active_window)
                break;

            auto const& info = tools.info_for(active_window);
            auto extra_data = std::static_pointer_cast<ExtraWindowData>(info.userdata());
            if (!extra_data)
                break;

            miral::WindowSpecification specification;
            if (extra_data->is_centered)
            {
                extra_data->is_centered = false;
                specification.top_left() = extra_data->original_placement.top_left;
                specification.size() = extra_data->original_placement.size;
            }
            else
            {
                auto const placement_rectangle = tools.active_output();
                int const center_x = placement_rectangle.top_left.x.as_value() + placement_rectangle.size.width.as_int() / 2;
                int const center_y = placement_rectangle.top_left.y.as_value() + placement_rectangle.size.height.as_int() / 2;
                int const placement_x = center_x - active_window.size().width.as_int() / 2;
                int const placement_y = center_y - active_window.size().height.as_int() / 2;
                extra_data->is_centered = true;
                specification.top_left() = mir::geometry::Point(placement_x, placement_y);
            }

            tools.modify_window(active_window, specification);
            return true;
        }
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
    specification.userdata() = std::make_shared<ExtraWindowData>(placement);
    return specification;
}

void MirlyWindowManager::handle_modify_window(miral::WindowInfo& window_info, miral::WindowSpecification const& modifications)
{
    miral::WindowSpecification specification = modifications;
    if (specification.state() == mir_window_state_maximized)
    {
        specification.state().consume();
        specification.top_left().consume();
        specification.size().consume();
    }

    MinimalWindowManager::handle_modify_window(window_info, specification);
}
