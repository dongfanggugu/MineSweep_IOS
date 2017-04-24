#ifndef FTILE_H
#define FTILE_H

#include "cocos2d.h"

class FTile : public cocos2d::Ref
{
public:

	~FTile();
	bool open;
	bool mine;
	bool flag;

	bool init();
	CREATE_FUNC(FTile);
};



#endif // !FTILE_H
