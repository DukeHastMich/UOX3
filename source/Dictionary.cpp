// Dictionary.cpp: implementation of the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "uox3.h"
#include "Dictionary.h"
#include "speech.h"
#include "scriptc.h"
#include "ssection.h"

namespace UOX
{

CDictionaryContainer *Dictionary;

const SI32 dictCANTOPEN			= -1;
const SI32 dictDUPESECTION		= -2;
const SI32 dictDUPEOPENBRACE	= -3;
const SI32 dictNOOPENBRACE		= -4;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDictionary::CDictionary() : IsValid( false )
{
	Language			= "ZRO";
	PathToDictionary	= "./dictionary.ZRO";
}

CDictionary::~CDictionary()
{
}

CDictionary::CDictionary( const std::string& filepath, const std::string& language )
{
	CDictionary();	//	Call default constructor for this Class
#pragma note( "Is the above line valid? You supposedly cannot call constructors from constructors in c++..." )

	if( language.empty() )
		Language = "ZRO";
	else
		Language = language;

	//	Load the path
	if( filepath.empty() )
		PathToDictionary = "dictionary.ZRO";
	else
		PathToDictionary = filepath;
}

const UI16 DICT_MAX_TEXTBUFFERSIZE = 1024;

SI32 CDictionary::LoadDictionary( void )
{
	SI32 count	= 0;
	IsValid		= false;
	if( FileExists( PathToDictionary ) )
	{
		Script *dictData = new Script( PathToDictionary, NUM_DEFS, false );
		if( dictData != NULL )
		{
			ScriptSection *dictSect = NULL;
			UString tag, data;
			for( dictSect = dictData->FirstEntry(); dictSect != NULL; dictSect = dictData->NextEntry() )
			{
				if( dictSect != NULL )
				{
					if( dictData->EntryName().substr( 0, 10 ) != "DICTIONARY" )	// verify it is a dictionary entry
						continue;
					for( tag = dictSect->First(); !dictSect->AtEnd(); tag = dictSect->Next() )
					{
						data					= dictSect->GrabData();
						Text2[tag.toULong()]	= data.stripWhiteSpace();
						++count;
					}
				}
			}
			delete dictData;
			dictData = NULL;
		}
		IsValid = true;
		Console.Print( " " );
		Console.MoveTo( 15 );
		Console << "Dictionary." << Language;
		Console.PrintSpecial( CGREEN, "loaded" );
	}
	else
		count = dictCANTOPEN;

	return count;
}

void CDictionary::ShowList( void )
{
	Console << "[Testing]" << myendl;
	std::map< long, std::string >::const_iterator toFind = Text2.begin();
	while( toFind != Text2.end() )
	{
		Console << (SI32)(toFind->first) << ") " << toFind->second << myendl;
		++toFind;
	}
}

std::string CDictionary::operator[]( SI32 Num )
{
	return GetEntry( Num );
}

void CDictionary::SetValid( const bool newVal )
{
	IsValid = newVal;
}

bool CDictionary::GetValid( void ) const
{
	return IsValid;
}

size_t CDictionary::NumberOfEntries( void ) const
{
	return Text2.size();
}

std::string CDictionary::GetEntry( const SI32 Num )
{
	std::string rvalue;
	if( IsValid )
	{
		std::map< long, std::string >::const_iterator toFind = Text2.find( Num );
		if( toFind != Text2.end() )
			rvalue = toFind->second;
		else
			Console.Warning( "Dictionary Reference %i not found in \"%s\"", Num, PathToDictionary.c_str() );
	}
	return rvalue;
}

CDictionaryContainer::CDictionaryContainer() : defaultLang( ZERO )
{
	std::string buildName;
	for( UI16 i = (UI16)DL_UNKNOWN; i < (UI16)DL_COUNT; ++i )
	{
		buildName = cwmWorldState->ServerData()->Directory( CSDDP_DICTIONARIES ) + "dictionary." + DistinctLanguageNames[i];
		dictList[i] = new CDictionary( buildName, DistinctLanguageNames[i] );
	}
}

CDictionaryContainer::CDictionaryContainer( const std::string& filepath ) : defaultLang( ZERO )
{
	std::string buildName;
	for( UI16 i = (UI16)DL_UNKNOWN; i < (UI16)DL_COUNT; ++i )
	{
		buildName = filepath + "dictionary." + DistinctLanguageNames[i];
		dictList[i] = new CDictionary( buildName, DistinctLanguageNames[i] );
	}
}

CDictionaryContainer::~CDictionaryContainer()
{
	for( UI16 i = (UI16)DL_UNKNOWN; i < (UI16)DL_COUNT; ++i )
		delete dictList[i];
}

SI32 CDictionaryContainer::LoadDictionary( void )
{
	SI32 rvalue = 0;
	for( UI16 i = (UI16)DL_UNKNOWN; i < (UI16)DL_COUNT; ++i )
	{
		dictList[i]->LoadDictionary();
	}
	if( !dictList[LanguageCodesLang[ZERO]]->GetValid() )
	{
		Console.Error( "Dictionary.ZRO is bad or nonexistant" );
		Shutdown( FATAL_UOX3_BAD_DEF_DICT );
		rvalue = -1;
	}
	return rvalue;
}

std::string CDictionaryContainer::operator[]( const SI32 Num )
{
	return GetEntry( Num );
}

std::string CDictionaryContainer::GetEntry( const SI32 Num, const UnicodeTypes toDisp )
{
	std::string RetVal;
	const DistinctLanguage mLanguage = LanguageCodesLang[toDisp];

	if( mLanguage < DL_COUNT )
		RetVal = dictList[mLanguage]->GetEntry( Num );
	if( RetVal.empty() && toDisp != defaultLang )//if we are using a diff language, and the entry wasn't found, try the ZERO before we flat out return null
		RetVal = dictList[LanguageCodesLang[defaultLang]]->GetEntry( Num );

	return RetVal;
}

void CDictionaryContainer::SetDefaultLang( const UnicodeTypes newType )
{
	if( dictList[LanguageCodesLang[newType]]->GetValid() )
        defaultLang = newType;
}

}
