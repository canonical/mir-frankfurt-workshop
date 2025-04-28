#define MIR_LOG_COMPONENT "mirly-main"

#include <mir/log.h>

#include <miral/runner.h>
#include <miral/set_window_management_policy.h>
#include <miral/minimal_window_manager.h>
#include <miral/external_client.h>
#include <miral/append_event_filter.h>
#include <miral/toolkit_event.h>
#include <xkbcommon/xkbcommon-keysyms.h>

int main(int argc, char const* argv[])
{
   mir::log_info("Running Mirly");
   miral::MirRunner runner{argc, argv};
   miral::ExternalClientLauncher external_client_launcher;

   auto const open_terminal = [&](MirEvent const* event)
    {
        if (miral::toolkit::mir_event_get_type(event) != mir_event_type_input)
           return false;

        MirInputEvent const* input_event =  
            miral::toolkit::mir_event_get_input_event(event);
        if (miral::toolkit::mir_input_event_get_type(input_event)
               != mir_input_event_type_key)
            return false;

        MirKeyboardEvent const* kev =  miral::toolkit::mir_input_event_get_keyboard_event(input_event);

        if (miral::toolkit::mir_keyboard_event_action(kev) != mir_keyboard_action_down)
          return false;

        MirInputEventModifiers mods = miral::toolkit::mir_keyboard_event_modifiers(kev);
        if (!(mods & mir_input_event_modifier_alt) || !(mods & mir_input_event_modifier_ctrl))
            return false;
  
        std::string terminal_cmd = "konsole"; // Replace with your terminal command
        switch (miral::toolkit::mir_keyboard_event_keysym(kev))
        {
            case XKB_KEY_t:
            case XKB_KEY_T:
                external_client_launcher.launch({terminal_cmd});
                return true;
            default:
                return false;
        }
    };

   return runner.run_with(
    {
        miral::set_window_management_policy<miral::MinimalWindowManager>(),
        external_client_launcher,
        miral::AppendEventFilter(open_terminal)
    });

}
