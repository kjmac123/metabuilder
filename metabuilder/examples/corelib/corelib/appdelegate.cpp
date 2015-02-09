#include "corelib/core.h"
#include "corelib/appdelegate.h"

CoreAppDelegate::CoreAppDelegate()
{
    
}

CoreAppDelegate::~CoreAppDelegate()
{
    
}

void CoreAppDelegate::OnInit(void* initData)
{
    Core_Init(initData);
    MB_LOGINFO("CoreAppDelegate::Init");
}

void CoreAppDelegate::OnShutdown()
{
    MB_LOGINFO("CoreAppDelegate::Shutdown");
}

void CoreAppDelegate::OnPause()
{
    MB_LOGINFO("CoreAppDelegate::Pause");
}

void CoreAppDelegate::OnResume()
{
    MB_LOGINFO("CoreAppDelegate::Resume");
}
