#include "uox3.h"
#include "classes.h"
#include "regions.h"

#include "ObjectFactory.h"

namespace UOX
{

cMapRegion *MapRegion;

void LoadChar( std::ifstream& readDestination )
{
	CChar *x = static_cast< CChar * >(ObjectFactory::getSingleton().CreateBlankObject( OT_CHAR ));
	if( x == NULL ) 
		return;
	if( !x->Load( readDestination ) )
	{
		x->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( x );
	}
}
void LoadItem( std::ifstream& readDestination )
{
	CItem *x = static_cast< CItem * >(ObjectFactory::getSingleton().CreateBlankObject( OT_ITEM ));
	if( x == NULL ) 
		return;
	if( !x->Load( readDestination ) )
	{
		x->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( x );
	}
}
void LoadMulti( std::ifstream& readDestination )
{
	CMultiObj *ourHouse = static_cast< CMultiObj * >(ObjectFactory::getSingleton().CreateBlankObject( OT_MULTI ));
	if( !ourHouse->Load( readDestination ) )	// if no load, DELETE
	{
		ourHouse->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( ourHouse );
	}
}

void LoadBoat( std::ifstream& readDestination )
{
	CBoatObj *ourBoat = static_cast< CBoatObj * >(ObjectFactory::getSingleton().CreateBlankObject( OT_BOAT ));
	if( !ourBoat->Load( readDestination ) ) // if no load, DELETE
	{
		ourBoat->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( ourBoat );
	}
}

void LoadSpawnItem( std::ifstream& readDestination )
{
	CSpawnItem *ourSpawner = static_cast< CSpawnItem * >(ObjectFactory::getSingleton().CreateBlankObject( OT_SPAWNER ));
	if( !ourSpawner->Load( readDestination ) ) // if no load, DELETE
	{
		ourSpawner->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( ourSpawner );
	}
}

//o--------------------------------------------------------------------------o
//|	Function			-	void SubRegion::SaveToDisk( std::ofstream& writeDestination, std::ofstream &houseDestination )
//|	Date				-	Unknown
//|	Developers		-	Unknown
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Save all items and characters inside a subregion
//|								reworked SaveChar from WorldMain to deal with pointer based stuff in region rather than index based stuff in array
//|								Also saves out all data regardless (in preparation for a simple binary save)
//|
//|
//|								Modified the loops, we cannot use our default First() Next() Finished() functions as RemoveChar() & RemoveItem() call
//|								vector.erase() which will automatically shift the data down by one (and update the iterator for us).
//o--------------------------------------------------------------------------o
void SubRegion::SaveToDisk( std::ofstream& writeDestination, std::ofstream &houseDestination )
{
	charData.Push();
	for( CChar* charToWrite = charData.First(); !charData.Finished(); charToWrite = charData.Next() )
	{
		if( !ValidateObject( charToWrite ) )
		{
			charData.Remove( charToWrite );
			continue;
		}
		//if( !charToWrite->IsNpc() && charToWrite->GetAccount() != -1 ) {
#if defined( _MSC_VER )
		#pragma todo( "PlayerHTML Dumping needs to be reimplemented" )
#endif
		//DumpPlayerHTML( charToWrite );
		if( charToWrite->ShouldSave() ) 
			charToWrite->Save( writeDestination );
	}
	charData.Pop();
	itemData.Push();
	for( CItem *itemToWrite = itemData.First(); !itemData.Finished(); itemToWrite = itemData.Next() )
	{
		if( !ValidateObject( itemToWrite ) )
		{
			itemData.Remove( itemToWrite );
			continue;
		}
		if( itemToWrite->ShouldSave() )
		{
			if( itemToWrite->GetObjType() == OT_MULTI )
			{
				CMultiObj *iMulti = static_cast<CMultiObj *>(itemToWrite);
				iMulti->Save( houseDestination );
			}
			else if( itemToWrite->GetObjType() == OT_BOAT )
			{
				CBoatObj *iBoat = static_cast< CBoatObj * >(itemToWrite);
				iBoat->Save( houseDestination );
			}
			else
				itemToWrite->Save( writeDestination );
		}
	}
	itemData.Pop();
}

//o--------------------------------------------------------------------------o
//|	Function		-	LoadFromDisk( std::ifstream& filename )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Loads in items/NPCs from disk in filename
//o--------------------------------------------------------------------------o
void SubRegion::LoadFromDisk( std::ifstream& filename )
{
	char line[1024];
	while( !filename.eof() && !filename.fail() )
	{
		filename.getline( line, 1024 );
		UString sLine( line );
		sLine = sLine.removeComment().stripWhiteSpace();
		if( sLine.substr( 0, 1 ) == "[" )	// in a section
		{
			sLine = sLine.substr( 1, sLine.size() - 2 );
			sLine = sLine.upper().stripWhiteSpace();
			if( sLine == "CHARACTER" )
				LoadChar( filename );
			else if( sLine == "ITEM" )
				LoadItem( filename );
			else if( sLine == "HOUSE" )
				LoadMulti( filename );
			else if( sLine == "BOAT" )
				LoadBoat( filename );
			else if( sLine == "SPAWNITEM" )
				LoadSpawnItem( filename );
		}
		else if( sLine == "---REGION---" )	// end of region
			return;
	}
}

CDataList< CItem * > * SubRegion::GetItemList( void )
{
	return &itemData;
}

CDataList< CChar * > * SubRegion::GetCharList( void )
{
	return &charData;
}

//o--------------------------------------------------------------------------o
//|	Function		-	cMapRegion constructor
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	This function is does basically what the name implies
//o--------------------------------------------------------------------------o
cMapRegion::cMapRegion()
{
	for( UI08 i = 0; i < (UI08)UOMT_COUNT; ++i )
	{
		upperArrayX[i] = MapWidths[i] / MapColSize;
		upperArrayY[i] = MapHeights[i] / MapRowSize;
	}
}; //constructor

//o--------------------------------------------------------------------------o
//|	Function		-	cMapRegion constructor
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	This function is does basically what the name implies
//o--------------------------------------------------------------------------o
cMapRegion::~cMapRegion()
{
	
}; //destructor

//o--------------------------------------------------------------------------o
//|	Function		-	SubRegion *GetCell( SI16 x, SI16 y )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns the subregion that x,y is in
//o--------------------------------------------------------------------------o
SubRegion *cMapRegion::GetCell( SI16 x, SI16 y, UI08 worldNumber )
{
	return GetGrid( GetGridX( x ), GetGridY( y ), worldNumber );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool AddItem( CItem *nItem )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Adds nItem to the proper SubRegion
//o--------------------------------------------------------------------------o
bool cMapRegion::AddItem( CItem *nItem )
{
	if( !ValidateObject( nItem ) )
		return false;
	SubRegion *cell = GetCell( nItem->GetX(), nItem->GetY(), nItem->WorldNumber() );
	if( cell == &overFlow )
	{
		overFlow.GetItemList()->Add( nItem );
		return false;
	}
	return cell->GetItemList()->Add( nItem );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool RemoveItem( CItem *nItem )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Removes nItem from it's CURRENT SubRegion
//|					-	Do this before adjusting the location
//o--------------------------------------------------------------------------o
bool cMapRegion::RemoveItem( CItem *nItem )
{
	if( !ValidateObject( nItem ) )
		return false;
	SubRegion *cell = GetCell( nItem->GetX(), nItem->GetY(), nItem->WorldNumber() );
	if( cell == &overFlow )
	{
		overFlow.GetItemList()->Remove( nItem );
		return false;
	}
	return cell->GetItemList()->Remove( nItem );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool AddChar( CChar *toAdd )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Adds toAdd to the proper SubRegion
//o--------------------------------------------------------------------------o
bool cMapRegion::AddChar( CChar *toAdd )
{
	if( !ValidateObject( toAdd ) )
		return false;
	SubRegion *cell = GetCell( toAdd->GetX(), toAdd->GetY(), toAdd->WorldNumber() );
	if( cell == &overFlow )
	{
		overFlow.GetCharList()->Add( toAdd );
		return false;
	}
	return cell->GetCharList()->Add( toAdd );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool RemoveChar( CChar *toRemove )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Removes toRemove from it's CURRENT SubRegion
//|					-	Do this before adjusting the location
//o--------------------------------------------------------------------------o
bool cMapRegion::RemoveChar( CChar *toRemove )
{
	if( !ValidateObject( toRemove ) )
		return false;
	SubRegion *cell = GetCell( toRemove->GetX(), toRemove->GetY(), toRemove->WorldNumber() );
	if( cell == &overFlow )
	{
		overFlow.GetCharList()->Remove( toRemove );
		return false;
	}
	return cell->GetCharList()->Remove( toRemove );
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI16 GetGridX( SI16 x )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Find the Column of SubRegion we want based on location
//o--------------------------------------------------------------------------o
SI16 cMapRegion::GetGridX( SI16 x )
{
	return static_cast<SI16>(x / MapColSize);
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI16 GetGridY( SI16 y )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Finds the Row of SubRegion we want based on location
//o--------------------------------------------------------------------------o
SI16 cMapRegion::GetGridY( SI16 y )
{
	return static_cast<SI16>(y / MapRowSize);
}

//o--------------------------------------------------------------------------o
//|	Function		-	Save()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Saves out each SubRegion
//o--------------------------------------------------------------------------o
void cMapRegion::Save( void )
{
	const long AreaX				= UpperX / 8;	// we're storing 8x8 grid arrays together
	const long AreaY				= UpperY / 8;
	std::ofstream writeDestination, houseDestination;
	const int onePercent			= (int)((float)(AreaX*AreaY*8*8*NumberOfWorlds)/100.0f);
	const char blockDiscriminator[] = "\n\n---REGION---\n\n";
	const char binBlockDisc			= (char)0xFF;
	int count						= 0;
	const UI08 Version				= 3;
	int s_t							= getclock();

	Console << "Saving Character and Item Map Region data...   ";
	Console.TurnYellow();
	Console << "0%";

	std::string basePath = cwmWorldState->ServerData()->Directory( CSDDP_SHARED );
	std::string filename = basePath + "house.wsc";
	
	houseDestination.open( filename.c_str() );

	for( SI32 counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
	{
		long baseX = counter1 * 8;
		for( SI32 counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
		{
			long baseY	= counter2 * 8;								// calculate x grid offset
			filename	= basePath + UString::number( counter1 ) + "." + UString::number( counter2 ) + ".wsc";;	// let's name our file
			writeDestination.open( filename.c_str() );

			if( !writeDestination ) 
			{
				Console.Error( 1, "Failed to open %s for writing", filename.c_str() );
				continue;
			}

			for( UI08 xCnt = 0; xCnt < 8; ++xCnt )					// walk through each part of the 8x8 grid, left->right
			{
				for( UI08 yCnt = 0; yCnt < 8; ++yCnt )				// walk the row
				{
					for( UI08 wCnt = 0; wCnt < NumberOfWorlds; ++wCnt )
					{
						++count;
						if( count%onePercent == 0 )
						{
							if( count/onePercent <= 10 )
								Console << "\b\b" << (UI32)(count/onePercent) << "%";
							else if( count/onePercent <= 100 )
								Console << "\b\b\b" << (UI32)(count/onePercent) << "%";
						}
						internalRegions[baseX + xCnt][baseY + yCnt][wCnt].SaveToDisk( writeDestination, houseDestination );

						writeDestination << blockDiscriminator;
					}
				}
			}
			writeDestination.close();
		}
	}
	houseDestination.close();

	filename = basePath + "overflow.wsc";
	writeDestination.open( filename.c_str() );

	if( writeDestination.is_open() )
	{
		overFlow.SaveToDisk( writeDestination, writeDestination );
		writeDestination.close();
	}
	else
	{
		Console.Error( 1, "Failed to open %s for writing", filename.c_str() );
		return;
	}
	
	Console << "\b\b\b\b";
	Console.PrintDone();

	UI32 e_t = getclock();
	Console.Print( "World saved in %.02fsec\n", ((float)(e_t-s_t))/1000.0f );
}

bool PostLoadFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	if( ValidateObject( a ) )
	{
		if( !a->isFree() )
			a->PostLoadProcessing();
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	Load()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Loads in each SubRegion
//o--------------------------------------------------------------------------o
void cMapRegion::Load( void )
{
	const long AreaX		= UpperX / 8;	// we're storing 8x8 grid arrays together
	const long AreaY		= UpperY / 8;
	const int onePercent	= (int)((float)(AreaX*AreaY*8*8*NumberOfWorlds)/100.0f);
	int count				= 0;
	std::ifstream readDestination;
	UI08 Version			= 0;

	Console.TurnYellow();
	Console << "0%";
	int s_t					= getclock();
	std::string basePath	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED );
	std::string filename;
	for( int counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
	{
		long baseX = counter1 * 8;
		for( int counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
		{
			long baseY	= counter2 * 8;								// calculate x grid offset
			filename	= basePath + UString::number( counter1 ) + "." + UString::number( counter2 ) + ".wsc";	// let's name our file
			readDestination.open( filename.c_str() );					// let's open it 

			if( readDestination.eof() || readDestination.fail() )
			{
				readDestination.close();
				readDestination.clear();
				continue;
			}
			for( UI08 xCnt = 0; xCnt < 8; ++xCnt )					// walk through each part of the 8x8 grid, left->right
			{
				long writeLong = baseX + xCnt;						// we know which column we're on
				for( UI08 yCnt = 0; yCnt < 8; ++yCnt )				// walk the row
				{
					for( UI08 wCnt = 0; wCnt < NumberOfWorlds; ++wCnt )
					{
						++count;
						if( count%onePercent == 0 )
						{
							if( count/onePercent <= 10 )
								Console << "\b\b" << (UI32)(count/onePercent) << "%";
							else if( count/onePercent <= 100 )
								Console << "\b\b\b" << (UI32)(count/onePercent) << "%";
						}
						internalRegions[writeLong][baseY + yCnt][wCnt].LoadFromDisk( readDestination );	// load it in as ASCII
					}
				}
			}
			readDestination.close();
		}
	}

	Console.TurnNormal();
	Console << "\b\b\b";
	Console.PrintDone();

	filename = basePath + "overflow.wsc";
	std::ifstream flowDestination( filename.c_str() );
	overFlow.LoadFromDisk( flowDestination );
	flowDestination.close();

	filename = basePath + "house.wsc";
	std::ifstream houseDestination( filename.c_str() );
	LoadHouseMulti( houseDestination );

	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_MULTI, b, NULL, &PostLoadFunctor );
	ObjectFactory::getSingleton().IterateOver( OT_CHAR, b, NULL, &PostLoadFunctor );
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &PostLoadFunctor );
	houseDestination.close();

	int e_t = getclock();
	Console.Print( "ASCII world loaded in %.02fsec\n", ((float)(e_t-s_t))/1000.0f );
}

int cMapRegion::GetGridIndex( SI16 x, SI16 y )
{
	SI16 trgX = GetGridX( x );
	SI16 trgY = GetGridY( y );
	return ( trgX + trgY * UpperX );	// x offset + y * num cols per row
}

SubRegion *cMapRegion::GetGrid( int xOffset, int yOffset, UI08 worldNumber )
{
	if( xOffset < 0 || xOffset >= upperArrayX[worldNumber] || yOffset < 0 || yOffset >= upperArrayY[worldNumber] )
		return &overFlow;
	return &internalRegions[xOffset][yOffset][worldNumber];
}

void cMapRegion::LoadHouseMulti( std::ifstream &houseDestination )
{
	char line[1024];
	while( !houseDestination.eof() && !houseDestination.fail() )
	{
		houseDestination.getline( line, 1024 );
		UString sLine( line );
		sLine = sLine.removeComment().stripWhiteSpace();
		if( sLine.substr( 0, 1 ) == "[" )	// in a section
		{
			sLine = sLine.substr( 1, sLine.size() - 2 );
			sLine = sLine.upper().stripWhiteSpace();
			if( sLine == "CHARACTER" )
				LoadChar( houseDestination );
			else if( sLine == "ITEM" )
				LoadItem( houseDestination );
			else if( sLine == "HOUSE" )
				LoadMulti( houseDestination );
			else if( sLine == "BOAT" )
				LoadBoat( houseDestination );
		}
		else if( sLine == "---REGION---" )	// end of region
			break;
	}
}

bool cMapRegion::Add( CBaseObject *toAdd )
{
	if( !ValidateObject( toAdd ) )
		return false;
	if( toAdd->CanBeObjType( OT_CHAR ) )
		return AddChar( static_cast< CChar *>(toAdd) );
	else
		return AddItem( (CItem *)(toAdd) );
}
bool cMapRegion::Remove( CBaseObject *toRemove )
{
	if( !ValidateObject( toRemove ) )
		return false;
	if( toRemove->CanBeObjType( OT_CHAR ) )
		return RemoveChar( static_cast< CChar *>(toRemove) );
	else
		return RemoveItem( (CItem *)(toRemove) );
}

REGIONLIST cMapRegion::PopulateList( CBaseObject *mObj )
{
	return PopulateList( mObj->GetX(), mObj->GetY(), mObj->WorldNumber() );
}

REGIONLIST cMapRegion::PopulateList( SI16 x, SI16 y, UI08 worldNumber )
{
	REGIONLIST nearbyRegions;
	bool isOnList	= false;
	SI16 xOffset	= MapRegion->GetGridX( x );
	SI16 yOffset	= MapRegion->GetGridY( y );
	for( SI08 counter1 = -1; counter1 <= 1; ++counter1 )
	{
		for( SI08 counter2 = -1; counter2 <= 1; ++counter2 )
		{
			isOnList			= false;
			SubRegion *MapArea	= GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( MapArea == NULL )
				continue;
			nearbyRegions.push_back( MapArea );
		}
	}
	return nearbyRegions;
}

}
