#include "PopupLayer.h"

USING_NS_CC;

PopupLayer::PopupLayer()
{
    mContentPadding = 0;
    mContentPaddingTop = 0;
    mPMenu = NULL;
    mCallBackListenner = NULL;
    mCallBack = NULL;
    mSpBackground = NULL;
    mLabelContentText = NULL;
    mLabelTitle = NULL;
}

PopupLayer::~PopupLayer()
{
    CC_SAFE_RELEASE(mPMenu);
    CC_SAFE_RELEASE(mSpBackground);
    CC_SAFE_RELEASE(mLabelContentText);
    CC_SAFE_RELEASE(mLabelTitle);

}

bool PopupLayer::init()
{
    if (!CCLayer::init())
    {
        return false;
    }

    setContentSize(Size(0, 0));

    CCMenu *menu = CCMenu::create();
    menu->setPosition(Point(0, 0));
    setMenuButton(menu);

    registerTouchDispatcher();

    return true;
}

void PopupLayer::registerTouchDispatcher()
{
    auto listenner = EventListenerTouchOneByOne::create();
    listenner->setSwallowTouches(true);
    listenner->onTouchBegan = [this](Touch *touch, Event *event) -> bool {
        return true;
    };

    //CCDirector::sharedDirector()->getEventDispatcher()->addEventListenerWithFixedPriority(listenner, -1);
    CCDirector::sharedDirector()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenner, this);
}

PopupLayer* PopupLayer::create(const char *backgroundImage)
{
    PopupLayer* popup = PopupLayer::create();
    popup->setSpriteBackground(CCSprite::create(backgroundImage));
    return popup;
}

void PopupLayer::setTitle(const char *title, int fontsize)
{
    CCLabelTTF *labelTitle = CCLabelTTF::create(title, "Arial", fontsize);
    labelTitle->setColor(ccc3(0, 0, 0));
    setLabelTitle(labelTitle);
}

void PopupLayer::setContentText(const char *text, int fontsize, int padding, int paddingTop)
{
    CCLabelTTF *labelContent = CCLabelTTF::create(text, "Arial", fontsize);
    labelContent->setColor(ccc3(0, 0, 0));
    setLabelContentText(labelContent);
    mContentPadding = padding;
    mContentPaddingTop = paddingTop;
}

void PopupLayer::setCallBackFunc(Ref *target, SEL_CallFuncN callfun)
{
    mCallBackListenner = target;
    mCallBack = callfun;
}

bool PopupLayer::addButton(const char *normalImage, const char *selectedImage, const char *title, int tag)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    Point center = Point(winSize.width / 2, winSize.height / 2);

    //create the button
    CCMenuItemImage *menuImage = CCMenuItemImage::create(
        normalImage, selectedImage, this, menu_selector(PopupLayer::buttonCallBack));
    menuImage->setTag(tag);
    menuImage->setPosition(center);

    //add text and set the position
    CCSize menuSize = menuImage->getContentSize();
    CCLabelTTF *ttf = CCLabelTTF::create(title, "Arial", 15);
    ttf->setColor(ccc3(0, 0, 0));
    ttf->setPosition(Point(menuSize.width / 2, menuSize.height / 2));
    menuImage->addChild(ttf);

    getMenuButton()->addChild(menuImage);

    return true;
}

/**
 *remove the dialog from parent
 *
 */
void PopupLayer::buttonCallBack(Ref *pSender)
{
    CCNode *node = dynamic_cast<CCNode *>(pSender);

    if (mCallBack && mCallBackListenner)
    {
        (mCallBackListenner->*mCallBack)(node);
    }

    this->removeFromParentAndCleanup(true);

}

void PopupLayer::onEnter()
{
    CCLayer::onEnter();

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    Point center = Point(winSize.width / 2, winSize.height / 2);
    CCSize contentSize;

    //if (getContentSize().equals(CCSizeZero))
   // {
        getSpriteBackground()->setPosition(center);
        this->addChild(getSpriteBackground(), 0, 0);
        contentSize = getSpriteBackground()->getTexture()->getContentSize();
    //}
   // else
   // {

   // }

    //add button, set the position by the amount of item
    this->addChild(getMenuButton());
    float btnWidth = contentSize.width / (getMenuButton()->getChildrenCount() + 1);
    Vector<Node *> array = getMenuButton()->getChildren();
    Ref *pRef = NULL;
    
    int i = 0;
    for (int i = 0; i < array.size(); i++)
    {
        Node *node = array.at(i);
        node->setPosition(Point(winSize.width / 2 - contentSize.width / 2
                          + btnWidth * (i + 1), winSize.height / 2 - contentSize.height / 3));
    }
    
    //show the title of the popup
    if (getLabelTitle())
    {
        getLabelTitle()->setPosition(ccpAdd(center, ccp(0, contentSize.height / 2 - 25.0f)));
        this->addChild(getLabelTitle());
    }
    
    //show the content text
    if (getLabelContentText())
    {
        CCLabelTTF *ltf = getLabelContentText();
        ltf->setPosition(center);
        ltf->setDimensions(CCSizeMake(contentSize.width - mContentPadding * 2, contentSize.height - mContentPaddingTop));
        ltf->setHorizontalAlignment(TextHAlignment::LEFT);
        this->addChild(ltf);
    }
    
    
    //pop up annimation
    CCSequence *popupActions = CCSequence::create(
                                                  CCScaleTo::create(0, 0),
                                                  CCScaleTo::create(0.2, 1.05),
                                                  CCScaleTo::create(0.1, 0.95),
                                                  CCScaleTo::create(0.1, 1.0), NULL);

    Point point = this->getAnchorPoint();
    log("dialog anchor x:%f y %f", point.x, point.y);
    
    this->runAction(popupActions);
}

//quit
void PopupLayer::onExit()
{
    CCLayer::onExit();
}