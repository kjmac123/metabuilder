#include "corelib/core.h"
#include "corelib/platform/platform.h"

class CoreAppDelegate
{
public:
    CoreAppDelegate();
    virtual ~CoreAppDelegate();
    
    virtual void OnLaunched();
    virtual void OnWillResignActive();
    virtual void OnDidEnterBackground();
    virtual void OnWillEnterForeground();
    virtual void OnDidBecomeActive();
    virtual void OnWillTerminate();
    
    virtual void Update();
};
