#import "AppViewController.h"

#include "corelib/core.h"
#include "app/mainview.h"

#import <OpenGLES/ES2/glext.h>

MainView* g_mainView;

@interface AppViewController () {
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation AppViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];

    g_mainView = new MainView();
    g_mainView->Init();
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
 
    g_mainView->Shutdown();
    
    delete g_mainView;
    g_mainView = NULL;
}

- (void)update
{
    g_mainView->Update();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    ViewSettings viewSettings;
    viewSettings.width = self.view.bounds.size.width;
    viewSettings.height = self.view.bounds.size.height;

    g_mainView->Render(viewSettings);
}


@end
