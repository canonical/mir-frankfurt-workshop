#ifndef MIRLY_WINDOW_MANAGER_H
#define MIRLY_WINDOW_MANAGER_H

#include <miral/minimal_window_manager.h>

class MirlyWindowManager : public miral::MinimalWindowManager
{
public:
    MirlyWindowManager(miral::WindowManagerTools const& tools);
    bool handle_keyboard_event(MirKeyboardEvent const* event) override;
};

#endif
