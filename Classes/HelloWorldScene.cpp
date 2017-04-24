#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Mines/Tiles.h"
#include "Views/PopupLayer.h"

#define MINE_SIZE 10
#define MAP_PADING 25

USING_NS_CC;

using namespace cocostudio::timeline;

int gPressedCount = 0;

Point gCurPoint;

HelloWorld::~HelloWorld()
{
    CC_SAFE_RELEASE(tiles);
    CC_SAFE_RELEASE(mTextlTimer);
}

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    /**  you can create scene with following comment code instead of using csb file.
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    **/
    
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    _preDistance = 0;
    _prePoint = Point(-1, -1);

	//create the tiles to manage the mine data
	tiles = Tiles::create();
    tiles->retain();
	int indexs[] = {8, 13, 25, 26, 32, 67, 89, 65, 45, 23};
	//int indexs[] = { 0, 1, 2, 3, 4 };
	std::vector<int> vecIndex(indexs, indexs + sizeof(indexs) / sizeof(int));

	tiles->setTileList(MINE_SIZE, vecIndex);
    
    
    auto rootNode = CSLoader::createNode("MainScene.csb");
	rootNode->setTag(0);

    
    //set the position of th map
    TMXTiledMap *map = static_cast<TMXTiledMap*>(rootNode->getChildByName("Map_1"));
    
    _map = map;
    
    map->setAnchorPoint(Point(0.5f, 0.5f));
    Size winSize = Director::getInstance()->getWinSize();
    Point center = Point(winSize.width / 2, winSize.height / 2);
    map->setPosition(center);

    float scaleX = (winSize.width - MAP_PADING * 2) / map->getContentSize().width;
    float scaleY = (winSize.height - MAP_PADING * 2) / map->getContentSize().height;
    _mineScale = MIN(scaleX, scaleY);
    map->setScale(_mineScale);
    
    //set the timer label
    mTextlTimer = static_cast<ui::Text*>(rootNode->getChildByName("text_timer"));
    
    mTextlTimer->setScale(2.0f);
    mTextlTimer->setTextColor(Color4B(255, 0, 0, 255));
    
    mTextlTimer->setString("0");
    mTextlTimer->setVisible(false);
    
    Point mapPositon = map->getPosition();
    
    float textX = winSize.width / 2;
    float textY = mapPositon.y + map->getContentSize().height * _mineScale / 2 + mTextlTimer->getContentSize().height * 2 / 2 + 30;
    
    mTextlTimer->setPosition(Point(textX, textY));
    
    //set the background
    Sprite *spriteBg = static_cast<Sprite*>(rootNode->getChildByName("Sprite_bg"));
    Size size = spriteBg->getContentSize();
    
    
    spriteBg->setScale(winSize.width / size.width, winSize.height / size.height);


    //add slider
    mSlider = ControlSlider::create("progress.png", "progress_passed.png", "slider.png");
    mSlider->setMinimumAllowedValue(0);
    mSlider->setMaximumAllowedValue(5);
    mSlider->setPosition(Point(winSize.width / 2, winSize.height - 50));
    Size content = mSlider->getContentSize();
    log("width:%f", content.width);
    mSlider->setValue(_mineScale / 5);
    this->addChild(mSlider, 1);

    mSlider->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::sliderCallback), Control::EventType::VALUE_CHANGED);


	//rootNode->getChildByName();

    this->addChild(rootNode);

	//setTouchEnabled(true);

	registerTouchListener();
    
    //registerMultiTouch();
    
    return true;
}

/**
 *  set the timer callback to modify the timer
 *
 *  @param dt <#dt description#>
 */
void HelloWorld::timerCallback(float dt)
{
    __String *strContent = __String::create(mTextlTimer->getString());
    int intContent = strContent->intValue();
    intContent++;
    __String *newStrContent = __String::createWithFormat("%d", intContent);
    mTextlTimer->setString(newStrContent->_string);
}

Point convertViewPointToMapCoord(TMXTiledMap *map, const Point &point, float scale)
{
    float mapMinX = map->getPosition().x - map->getAnchorPoint().x * map->getContentSize().width * scale;
    float mapMinY = map->getPosition().y - map->getAnchorPoint().y * map->getContentSize().height * scale;
    
    int indexX = -1;
    int indexY = -1;


    if (point.x - mapMinX < 0)
    {
        indexX = (point.x - mapMinX) / (map->getTileSize().width * scale) - 1;
    }
    else
    {
        indexX = (point.x - mapMinX) / (map->getTileSize().width * scale);
    }

    if (point.y - mapMinY < 0)
    {
        indexY = (point.y - mapMinY) / (map->getTileSize().height * scale) - 1;
    }
    else
    {
        indexY = (point.y - mapMinY) / (map->getTileSize().height * scale);
    }
    
    indexY = map->getMapSize().height - 1 - indexY;
    
    return Point(indexX, indexY);
}

void HelloWorld::pressedCount(float dt) 
{
	gPressedCount++;

	//if the touch time less than 1 second,short touch
	if (gPressedCount >= 2)
    {
		unschedule(schedule_selector(HelloWorld::pressedCount));

        TMXTiledMap *map = static_cast<TMXTiledMap *>(getChildByTag(0)->getChildByName("Map_1"));
        auto layer = map->getLayer("layer_main");

		//if the ftile is opened and the ftile is flaged, 
		//cancel the flag and close the ftile
		//if the ftile is closed and the ftile is not flaged,
		//flag the ftile and open it
		if (tiles->isOpen(gCurPoint))
		{
			if (tiles->isFlag(gCurPoint))
			{
				layer->setTileGID(11, gCurPoint);
				tiles->cancelFlag(gCurPoint);
				tiles->close(gCurPoint);
			}
		}
		else
		{
			//when the FTile is colsed, no FTile be flaged.
			if (!tiles->isFlag(gCurPoint))
			{
				layer->setTileGID(10, gCurPoint);
				tiles->setFlag(gCurPoint);
				tiles->open(gCurPoint);
			}
		}
        
        if (tiles->isGameFinish())
        {
            unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::timerCallback));
            start = false;

            std::string time = mTextlTimer->getString();
            time = "time:" + time;
            showDialog("Congratulations", time, "Play again", "Exit");
        }
        
        gCurPoint = Point(-1, -1);
        gPressedCount = 0;       
    }
}

bool HelloWorld::touchEnd(Touch *touch, Event *event)
{
    
    _prePoint = Point(-1, -1);

	unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::pressedCount));

	TMXTiledMap *map = static_cast<TMXTiledMap *>(getChildByTag(0)->getChildByName("Map_1"));

	Point clickPoint = convertViewPointToMapCoord(map, touch->getLocation(), _mineScale);


	if (clickPoint.x != gCurPoint.x || clickPoint.y != gCurPoint.y)
	{

		gCurPoint = Point(-1, -1);
		gPressedCount = 0;
		return true;
	}

    //start
    if (!start)
    {
        start = true;
        getTextTimer()->setVisible(true);
        schedule(CC_SCHEDULE_SELECTOR(HelloWorld::timerCallback), 1);
    }

	//if the touch time less than 0.5 second,short touch
	if (gPressedCount <= 1)
	{

        //if the FTile has been flaged, return
        if (tiles->isFlag(clickPoint))
        {
            gCurPoint = Point(-1, -1);
            gPressedCount = 0;
            return true;
        }

		auto layer = map->getLayer("layer_main");

		bool isMine = tiles->isMine(clickPoint);

        //if the FTile is a mine, game over!
		if (isMine)
		{
            unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::timerCallback));
            start = false;

			tiles->open(clickPoint);
			layer->setTileGID(9, clickPoint);
            showDialog("Sorry", "Game Over!", "Play again", "Exit");
		}
		else 
        {
			int index = 10 * clickPoint.x + clickPoint.y;
			tiles->computeMines(index, [&](Point point, int mineCount) {

				//callback to set point state
				if (-1 == mineCount)
				{
                    unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::timerCallback));
                    start = false;

                    tiles->open(point);
                    layer->setTileGID(9, point);
                    showDialog("Sorry", "Game Over!", "Play again", "Exit");
				}
                else if (0 == mineCount)
                {
                    layer->setTileGID(12, point);
                }
                else
				{
					layer->setTileGID(mineCount, point);
				}
			});
		}
	}
    
    if (tiles->isGameFinish())
    {
        unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::timerCallback));
        start = false;

        std::string time = mTextlTimer->getString();
        time = "time:" + time;
        showDialog("Congratulations", time, "Play again", "Exit");
    }
	
	gCurPoint = Point(-1, -1);
	gPressedCount = 0;
	return true;
}

void HelloWorld::registerTouchListener()
{
    this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	auto listener = EventListenerTouchOneByOne::create();
	

	listener->onTouchBegan = [this](Touch *touch, Event *event) -> bool {
        
        
		gCurPoint = convertViewPointToMapCoord(this->_map, touch->getLocation(), _mineScale);
        
        if (gCurPoint.x < 0 || gCurPoint.x >= MINE_SIZE
            || gCurPoint.y < 0 || gCurPoint.y >= MINE_SIZE)
        {
            return false;
        }

		schedule(CC_SCHEDULE_SELECTOR(HelloWorld::pressedCount), 0.2f);

		return true;
	};
    
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::touchEnd, this);

	listener->onTouchMoved = [this](Touch *touch, Event *event) -> bool {
        
		TMXTiledMap *map = static_cast<TMXTiledMap *>(getChildByTag(0)->getChildByName("Map_1"));

		Point curPoint = convertViewPointToMapCoord(map, touch->getLocation(), _mineScale);

        
        //move out of the init tile
		if (curPoint.x != gCurPoint.x || curPoint.y != gCurPoint.y)
		{
			this->unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::pressedCount));
			gPressedCount = 0;
			gCurPoint = Point(0, 0);
            
            //move the map
            
            if (_prePoint == Point(-1, -1))
            {
                _prePoint = touch->getLocation();
            }
            
            float deltaX = touch->getLocation().x - _prePoint.x;
            float deltaY = touch->getLocation().y - _prePoint.y;
            
            
            float x = _map->getPosition().x + deltaX;
            float y = _map->getPosition().y + deltaY;
      
            _map->setPosition(x, y);
            
            
            _prePoint = touch->getLocation();
            
		}

		return true;
	};


    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

//    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, -1);
}

void HelloWorld::showDialog(std::string title, std::string message, std::string leftText, std::string rightText)
{
    PopupLayer *dialog = PopupLayer::create("dialog.png");
    
    dialog->setTitle(title.c_str());
    
    dialog->setContentText(message.c_str());
    
    dialog->setCallBackFunc(this, callfuncN_selector(HelloWorld::dialogButtonCallback));
    
    dialog->addButton("button.png", "button.png", leftText.c_str(), 0);
    dialog->addButton("button.png", "button.png", rightText.c_str(), 1);
    
    this->addChild(dialog);
}

void HelloWorld::dialogButtonCallback(Node *button)
{
    int tag = button->getTag();
    if (0 == tag)
    {
        reset();
    }
    else if (1 == tag)
    {
        Director::getInstance()->end();
    }
}

void HelloWorld::reset()
{
    int indexs[] = { 8, 13, 25, 26, 32, 67, 89, 65, 45, 23 };
    std::vector<int> vecIndex(indexs, indexs + sizeof(indexs) / sizeof(int));
    tiles->reset(vecIndex);

    mTextlTimer->setString("0");

    TMXTiledMap *map = static_cast<TMXTiledMap*>(this->getChildByTag(0)->getChildByName("Map_1"));
    TMXLayer *layer = map->getLayer("layer_main");

    for (int i = 0; i < MINE_SIZE; i++)
    {
        for (int j = 0; j < MINE_SIZE; j++)
        {
            layer->setTileGID(11, Point(i, j));
        }
    }
}

/**
 *  register multi touch
 */
void HelloWorld::registerMultiTouch()
{
    auto listener = EventListenerTouchAllAtOnce::create();
    
    
    listener->onTouchesBegan = [this](const std::vector<Touch*> &touches, Event* event)->void {
        
        //单点触摸
//        if (1 == touches.size())
//        {
//            TMXTiledMap *map = static_cast<TMXTiledMap *>(getChildByTag(0)->getChildByName("Map_1"));
//            
//            gCurPoint = convertViewPointToMapCoord(map, touches.at(0)->getLocation(), _mineScale);
//            
//            if (gCurPoint.x < 0 || gCurPoint.x >= MINE_SIZE
//                || gCurPoint.y < 0 || gCurPoint.y >= MINE_SIZE)
//            {
//                return;
//            }
//            
//            //start
//            if (!start)
//            {
//                start = true;
//                getTextTimer()->setVisible(true);
//                schedule(CC_SCHEDULE_SELECTOR(HelloWorld::timerCallback), 1);
//            }
//            
//            schedule(CC_SCHEDULE_SELECTOR(HelloWorld::pressedCount), 0.2f);
//
//        }
        
    };
    
    listener->onTouchesMoved = [this](const std::vector<Touch*> &touches, Event *event)->void {

        if (2 == touches.size())
        {
            Point point1  = touches.at(0)->getLocation();
            Point point2 = touches.at(1)->getLocation();
            
            Size contentSize = _map->getContentSize();
            Point location = _map->getPosition();
            
            Point origin = Point(location.x - contentSize.width / 2, location.y - contentSize.height / 2);
            Rect rect = Rect(origin.x, origin.y, contentSize.width, contentSize.height);
            
            if (rect.containsPoint(point1) || rect.containsPoint(point2))
            {
                log("in the map");
                return;
            }
            
            
            double distance = sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
            
            if (0 == _preDistance)
            {
                _preDistance = distance;
            }
            
            double scale = distance / _preDistance;
            
            _preDistance = distance;
            
            _mineScale = _mineScale * scale;
            
            
            
            _map->setScale(_mineScale);
        }
//        else if (1 == touches.size())
//        {
//
//            
//            Point curPoint = convertViewPointToMapCoord(_map, touches.at(0)->getLocation(), _mineScale);
//            
//            
//            //move out of the init tile
//            if (curPoint.x != gCurPoint.x || curPoint.y != gCurPoint.y)
//            {
//                this->unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::pressedCount));
//                gPressedCount = 0;
//                gCurPoint = Point(0, 0);
//                
//                //move the map
//                
//                if (_prePoint == Point(-1, -1))
//                {
//                    _prePoint = touches.at(0)->getLocation();
//                }
//                
//                float deltaX = touches.at(0)->getLocation().x - _prePoint.x;
//                float deltaY = touches.at(0)->getLocation().y - _prePoint.y;
//                
//                
//                float x = _map->getPosition().x + deltaX;
//                float y = _map->getPosition().y + deltaY;
//                
//                _map->setPosition(x, y);
//                
//                
//                _prePoint = touches.at(0)->getLocation();
//            }
//            
//        }
        
    };
    
    listener->onTouchesEnded = [this](const std::vector<Touch*> &touches, Event *event)->void {
        
//        if (1 == touches.size())
//        {
//            touchEnd(touches.at(0), event);
//        }
        
        if (2 == touches.size())
        {
            this->_preDistance = 0;
        }
    };
    
    listener->onTouchesCancelled = [this](const std::vector<Touch*> &touches, Event *event)->void {
        this->_preDistance = 0;
    };
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _map);
    //Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listenner, -2);
}

void HelloWorld::sliderCallback(Ref *sender, Control::EventType eventType)
{
    ControlSlider *slider = (ControlSlider *)sender;
    float value = slider->getValue() * slider->getMaximumAllowedValue();
    if (value < 0.1)
    {
        value = 0.1;
    }

    log("value:%f", value);

    this->_mineScale = value;
    this->_map->setScale(value);
}
