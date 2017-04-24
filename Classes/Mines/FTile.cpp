#include "FTile.h"

USING_NS_CC;

bool FTile::init()
{
	open = false;
	mine = false;
	flag = false;
    
    return true;
}

FTile::~FTile()
{
	log("FTile delete");
}