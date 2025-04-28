#define MIR_LOG_COMPONENT "mirly-main"

#include <mir/log.h>

#include <miral/runner.h>
#include <miral/set_window_management_policy.h>
#include <miral/minimal_window_manager.h>

int main(int argc, char const* argv[])
{
   mir::log_info("Running Mirly");
   miral::MirRunner runner{argc, argv};

   return runner.run_with(
    {
        miral::set_window_management_policy<miral::MinimalWindowManager>()
    });

}
