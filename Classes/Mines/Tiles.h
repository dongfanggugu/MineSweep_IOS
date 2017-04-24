#ifndef TILES_H
#define TILES_H

#include "cocos2d.h"
#include "FTile.h"

class Tiles : public cocos2d::Ref
{
public:

	~Tiles();

    cocos2d::Vector<FTile*> mTileList;

	bool init();

	CREATE_FUNC(Tiles);

	void setTileList(int size, std::vector<int> indexs);
    
    void computeMines(int index, std::function<void(cocos2d::Point point, int mineCount)> callback);

	int getSize();

	//flag the FTile, and open the FTile
	void setFlag(cocos2d::Point point);

	//cancel the flag, and close the FTile;
	void cancelFlag(cocos2d::Point point);

	//has the FTile been flaged
	bool isFlag(cocos2d::Point point);

	//open the FTile
    void open(cocos2d::Point point);

	//close the FTile
	void close(cocos2d::Point point);
    
    //is the FTile open
    bool isOpen(cocos2d::Point point);
	
	//is the FTile a mine
	bool isMine(cocos2d::Point point);

    /**
     *  is the game finished
     *
     *  @return <#return value description#>
     */
    bool isGameFinish();

    bool reset(std::vector<int> indexs);
    
    
protected:
	int _size;
    int _mineSize;

    cocos2d::ValueVector getSurroundMineCount(std::vector<cocos2d::Point> surroundPoints);
};


#endif // !MINES_H
