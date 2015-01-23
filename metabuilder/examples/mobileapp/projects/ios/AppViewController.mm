#import "AppViewController.h"
#include "src/core.h"

@interface AppViewController () {
}

- (void)timerTick;
- (void)setup;
- (void)tearDown;
@property (weak, nonatomic) IBOutlet UILabel *label;

@end

@implementation AppViewController

NSTimer *m_timer;

- (void)timerTick
{
    double timeSeconds = Core_GetElapsedTimeSeconds();
    self.label.text = [NSString stringWithFormat:@"%.2f seconds", timeSeconds];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self setup];

    m_timer = [NSTimer scheduledTimerWithTimeInterval:
        0.025
        target:self
        selector:@selector(timerTick)
        userInfo: nil
        repeats:YES
    ];
}

- (void)dealloc
{    
    [self tearDown];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDown];
    }

}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)setup
{
    Core_Init();
}

- (void)tearDown
{
    Core_Shutdown();
}

@end
