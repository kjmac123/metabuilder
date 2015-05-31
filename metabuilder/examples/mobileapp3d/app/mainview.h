#ifndef MAINVIEW_H
#define MAINVIEW_H

class MainViewImpl;

struct ViewSettings
{
    float width;
    float height;
};

class MainView
{
public:
    MainView();
    ~MainView();
    
    void OnInit();
    void OnShutdown();
    void OnSurfaceChanged(const ViewSettings& viewSettings);
    void OnDrawFrame();
    
private:
    MainViewImpl* m_impl;
};

#endif