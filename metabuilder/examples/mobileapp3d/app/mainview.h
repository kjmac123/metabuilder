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
    
    void Init();
    void Shutdown();
    void Update();
    void Render(const ViewSettings& viewSettings);
    
private:
    MainViewImpl* m_impl;
};

#endif