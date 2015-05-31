#import "AppDelegate.h"
#include "corelib/appdelegate.h"

CoreAppDelegate* g_coreAppDelegate;

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    g_coreAppDelegate = new CoreAppDelegate;
    g_coreAppDelegate->OnInit(NULL);
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    g_coreAppDelegate->OnPause();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    g_coreAppDelegate->OnResume();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    g_coreAppDelegate->OnShutdown();
    delete g_coreAppDelegate;
}

@end
