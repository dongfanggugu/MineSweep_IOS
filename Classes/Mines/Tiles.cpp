#include "Tiles.h"

USING_NS_CC;




bool Tiles::init()
{
	return true;
}

void Tiles::setFlag(Point point)
{
	int index = point.x * _size + point.y;
	if (index < 0 || index > _size * _size - 1)
	{
		return;
	}

	FTile *ftile = mTileList.at(index);
	ftile->flag = true;
}

void Tiles::cancelFlag(Point point)
{
	int index = point.x * _size + point.y;
	if (index < 0 || index > _size * _size - 1)
	{
		return;
	}

	FTile *ftile = mTileList.at(index);
	ftile->flag = false;
}

bool Tiles::isFlag(Point point)
{
	int index = point.x * _size + point.y;
	if (index < 0 || index > _size * _size - 1)
	{
		return false;
	}
	FTile *ftile = mTileList.at(index);
	return ftile->flag;
}

void Tiles::open(cocos2d::Point point)
{
	int index = point.x * _size + point.y;
	if (index < 0 || index > _size * _size - 1)
	{
		return;
	}

	FTile *ftile = mTileList.at(index);
	ftile->open = true;
}

void Tiles::close(Point point)
{
	int index = point.x * _size + point.y;
	if (index < 0 || index > _size * _size - 1)
	{
		return;
	}
	FTile *ftile = mTileList.at(index);
	ftile->open = false;
}

bool Tiles::isOpen(Point point)
{
    int index = point.x * _size + point.y;
    if (index < 0 || index > _size * _size - 1)
    {
        return false;
    }
    FTile *ftile = mTileList.at(index);
    
    return ftile->open;
}


bool Tiles::isMine(Point point)
{
	int index = point.x * _size + point.y;
	if (index < 0 || index > _size * _size - 1)
	{
		return false;
	}

	FTile *ftile = mTileList.at(index);
	return ftile->mine;
}


/*
set the mine list
*/
void Tiles::setTileList(int size, std::vector<int> indexs)
{

	if (size <= 0)
	{
		return;
	}
	_size = size;

	int capacity = _size * _size;

	mTileList = Vector<FTile*>(capacity);

	for (int i = 0; i < mTileList.capacity(); i++)
	{
		FTile *tile = FTile::create();
		mTileList.pushBack(tile);
	}

	//set the mine location
	_mineSize = indexs.size();

	for (int i = 0; i < _mineSize; i++)
	{
		int index = indexs[i];
		if (index < 0 || index > size * size - 1)
		{
			return;
		}
        
		mTileList.at(index)->mine = true;
	}
}


/**
// get the surround points of the point selected
//
**/
std::vector<Point> getSurroundPoints(int row, int column, int size)
{
	//every Mine has at most 8 surround Point
	std::vector<Point> surroundPoints = std::vector<Point>();

	for (int i = row - 1; i <= row + 1; i++)
	{
		for (int j = column - 1; j <= column + 1; j++)
		{
			if (i < 0 || i >= size)
			{
				continue;
			}

			if (j < 0 || j >= size)
			{
				continue;
			}

			if (i == row && j == column)
			{
				continue;
			}

			surroundPoints.push_back(Point(i, j));
		}
	}

	return surroundPoints;
}

void Tiles::computeMines(int index, std::function<void(cocos2d::Point point, int mineCount)> callback)
{
    if (index < 0 || index > _size * _size - 1)
    {
        return;
    }

	FTile *ftile = mTileList.at(index);

    int row = index / _size;
    int column = index % _size;

    std::vector<Point> surroundPoints = getSurroundPoints(row, column, _size);
    ValueVector valueVector = getSurroundMineCount(surroundPoints);

    int mineCount = valueVector.at(0).asInt();
    int flagCount = valueVector.at(1).asInt();
    int flagNotMineCount = valueVector.at(2).asInt();
    int mineNotFlagIndex = valueVector.at(3).asInt();


    //if the FTile has been opened, 
	if (ftile->open)
	{
        if (0 == mineCount)
        {
            return;
        }

        //the flag is incorrect, game over!
        if (flagNotMineCount != 0)
        {
            if (mineNotFlagIndex != -1)
            {
                callback(Point(mineNotFlagIndex / _size, mineNotFlagIndex % _size), -1);
                return;
            }
            
        }

        //the mines of the surround have been flaged
        if (flagCount == mineCount)
        {
            for (int i = 0; i < surroundPoints.size(); i++)
            {
                Point point = surroundPoints.at(i);
                int curIndex = point.x * _size + point.y;
                if (!mTileList.at(curIndex)->open)
                {
                    computeMines(curIndex, callback);
                }
            }
        }

        return;
	}


    //the FTile is not open
    ftile->open = true;
    callback(Point(row, column), mineCount);

    if (0 == mineCount)
    {
        for (int i = 0; i < surroundPoints.size(); i++)
        {
            Point point = surroundPoints.at(i);
            int curIndex = point.x * _size + point.y;
            if (!mTileList.at(curIndex)->open)
            {
                computeMines(curIndex, callback);
            }
        }
    }
}

ValueVector Tiles::getSurroundMineCount(std::vector<Point> surroundPoints)
{

    ValueVector valueVector;
    int flagCount = 0;
	int mineCount = 0;
    int flagNotMineCount = 0;
    int mineNotFlagIndex = -1;

	for (int i = 0; i < surroundPoints.size(); i++)
	{
		Point point = surroundPoints.at(i);
		int index = point.x * _size + point.y;
		if (index < 0 || index > _size * _size - 1)
		{
			continue;
		}
		FTile *ftile = mTileList.at(index);
		if (ftile->mine)
		{
			mineCount++;
		}

        if (ftile->flag)
        {
            flagCount++;
        }

        if (!ftile->mine && ftile->flag)
        {
            flagNotMineCount++;
        }

        //record the index of the FTile which is a tile but not be flaged
        if (ftile->mine && !ftile->flag)
        {
            mineNotFlagIndex = index;
        }
	}

    valueVector.push_back(Value(mineCount));
    valueVector.push_back(Value(flagCount));
    valueVector.push_back(Value(flagNotMineCount));
    valueVector.push_back(Value(mineNotFlagIndex));

    return valueVector;
}

int Tiles::getSize()
{
	return _size;
}

Tiles::~Tiles()
{
	log("Tiles delete");
}


/**
 * compute the amount that the FTile is not
 * a mine and not be opened, and if there is
 * a no-mine FTile is flaged, return false,
 * if the amount equal the size of the mTileList
 * except the mines, return true
 */
bool Tiles::isGameFinish()
{
    int count = 0;
    for (int i = 0; i < mTileList.size(); i++)
    {
        FTile *ftile = mTileList.at(i);
        if (!ftile->mine && ftile->flag)
        {
            return false;
        }
        
        if (!ftile->mine && ftile->open)
        {
            count++;
        }
    }
    
    if (count == _size * _size - _mineSize)
    {
        return true;
    }
    
    return false;
}


bool Tiles::reset(std::vector<int> indexs)
{
    if (indexs.size() != _mineSize)
    {
        return false;
    }


    for (int i = 0; i < mTileList.size(); i++)
    {
        FTile *ftile = mTileList.at(i);
        ftile->mine = false;
        ftile->flag = false;
        ftile->open = false;
    }

    for (int i = 0; i < _mineSize; i++)
    {
        int index = indexs[i];
        if (index < 0 || index > _size * _size - 1)
        {
            return false;
        }

        mTileList.at(index)->mine = true;
    }

    return true;
}








