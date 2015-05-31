#include "corelib/core.h"
#include "corelib/platform/platform.h"

class CoreAppDelegate
{
public:
    CoreAppDelegate();
    virtual ~CoreAppDelegate();
    
    virtual void OnInit(void* initData);
    virtual void OnShutdown();
    virtual void OnPause();
    virtual void OnResume();
};
