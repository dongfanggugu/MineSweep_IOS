#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Mines/Tiles.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocostudio;
using namespace ui;

class HelloWorld : public cocos2d::Layer
{
public:

	~HelloWorld();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

	void registerTouchListener();
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	cocos2d::CCScheduler *sched;

	void pressedCount(float dt);

	bool touchEnd(cocos2d::Touch *touch, cocos2d::Event *event);

	Tiles *tiles;
    
    void showDialog(std::string title, std::string message, std::string leftText, std::string rightText);

    CC_SYNTHESIZE(cocos2d::ui::Text*, mTextlTimer, TextTimer);

    CC_SYNTHESIZE(cocos2d::extension::ControlSlider* , mSlider, Slider);

    void timerCallback(float dt);

    void sliderCallback(Ref *sender, cocos2d::extension::Control::EventType eventType);


    bool start;

    void dialogButtonCallback(cocos2d::Node *button);

    void reset();
    
    void registerMultiTouch();
    
private:
    double _mineScale;
    
    cocos2d::Point _point1;
    cocos2d::Point _point2;
    
    double _preDistance;
    
    cocos2d::Point _prePoint;
    
    cocos2d::TMXTiledMap *_map;
    
};

#endif // __HELLOWORLD_SCENE_H__
