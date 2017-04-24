#ifndef __POP_UP_LAYER_H__
#define __POP_UP_LAYER_H__


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class PopupLayer : public CCLayer
{
public:
    PopupLayer();
    ~PopupLayer();

    virtual bool init();


    //register the touch event dispatcher
    virtual void registerTouchDispatcher();

    static PopupLayer* create(const char *backgroundImage);

    //set the title
    void setTitle(const char *title, int fontsize = 20);

    //set the text
    void setContentText(const char *text, int fontsize = 20, int padding = 50, int paddingTop = 100);

    void setCallBackFunc(Ref *target, SEL_CallFuncN callfun);

    //add button
    bool addButton(const char *normalImage, const char *selectedImage, const char *title, int tag = 0);

    virtual void onEnter();

    virtual void onExit();

    CREATE_FUNC(PopupLayer);

private:

    void buttonCallBack(Ref *pSender);

    //the text padding
    int mContentPadding;

    int mContentPaddingTop;

    Ref *mCallBackListenner;

    SEL_CallFuncN mCallBack;

    //add UI controll
    CC_SYNTHESIZE_RETAIN(CCMenu*, mPMenu, MenuButton);
    CC_SYNTHESIZE_RETAIN(CCSprite*, mSpBackground, SpriteBackground);

    CC_SYNTHESIZE_RETAIN(CCLabelTTF*, mLabelTitle, LabelTitle);
    CC_SYNTHESIZE_RETAIN(CCLabelTTF*, mLabelContentText, LabelContentText);

};


#endif // !__POP_UP_LAYER_H__