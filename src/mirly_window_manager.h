#ifndef MIRLY_WINDOW_MANAGER_H
#define MIRLY_WINDOW_MANAGER_H

#include <miral/minimal_window_manager.h>
// 
class MirlyWindowManager : public miral::MinimalWindowManager
{
public:
    MirlyWindowManager(miral::WindowManagerTools const& tools);
    bool handle_keyboard_event(MirKeyboardEvent const* event) override;
    auto place_new_window(
        miral::ApplicationInfo const& app_info,
        miral::WindowSpecification const& requested_specification) -> miral::WindowSpecification override;
    void handle_modify_window(miral::WindowInfo& window_info, miral::WindowSpecification const& modifications) override;

private:
    int window_count = 0;
};

#endif
