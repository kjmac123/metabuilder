#import "AppDelegate.h"
#include "corelib/appdelegate.h"

CoreAppDelegate* g_coreAppDelegate;

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    g_coreAppDelegate = new CoreAppDelegate;
    g_coreAppDelegate->OnLaunched();
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    g_coreAppDelegate->OnWillResignActive();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    g_coreAppDelegate->OnDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    g_coreAppDelegate->OnWillEnterForeground();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    g_coreAppDelegate->OnWillResignActive();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    g_coreAppDelegate->OnWillTerminate();
    delete g_coreAppDelegate;
}

@end
