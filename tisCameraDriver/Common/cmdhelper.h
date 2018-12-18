
#ifndef CMDHELPER_H_INC_
#define CMDHELPER_H_INC_

#pragma once

#include <vector>
#include <string>
#include <iostream>

/** This function creates an array of strings from an ImagingControl list.
 * @param list_ The list from which you want to get the description strings.
 * @return An array of strings.
 */
template<class TList>
std::vector<std::string>		toStringArray( const TList& list_ )
{
	std::vector<std::string> arr;
	arr.reserve( list_.size() );
	for( TList::const_iterator it = list_.begin(); it != list_.end();  ++it )
	{
		arr.push_back( it->toString() );
	}
	return arr;
}

/**	This function creates an array of strings from two delimiting iterators
 *	of a sequence of objects having a toString() method.
 *	@param begin Start of the sequence
 *	@param end End of the sequence
 *	@return vector of strings containing the names of the items in the sequence.
 */
template<class TIter>
std::vector<std::string>		toStringArray( TIter begin, TIter end )
{
	std::vector<std::string> arr;

	arr.reserve( std::distance( begin, end ) );
	for( TIter it = begin; it != end; ++it )
	{
		arr.push_back( it->toString() );
	}

	return arr;
}

template<class TIter>
std::vector<std::wstring>		toStringArrayW( TIter begin, TIter end )
{
	std::vector<std::wstring> arr;

	arr.reserve( std::distance( begin, end ) );
	for( TIter it = begin; it != end; ++it )
	{
		arr.push_back( it->toStringW() );
	}

	return arr;
}

/** This function creates an array of strings from an ImagingControl list pointer.
 * @param pList The list pointer you want to get the description strings.
 * @return When either pList is == 0 or pList is empty, then the array returned is empty.
 */
template<class TListPtr>
std::vector<std::string>		toStringArrayPtr( const TListPtr& pList )
{
	if( pList == 0 )		return std::vector<std::string>();
	return toStringArray( *pList );
}

/** This function creates an array of strings from an ImagingControl list, which consists of pointers to 
 * ImagingControl objects.
 * @param list_ The list of pointers from which you want to get the description strings.
 * @return An array of strings.
 */
template<class TList>
std::vector<std::string>		toStringArrayPtrList( const TList& list_ )
{
	std::vector<std::string> arr;

	arr.reserve( list_.size() );
	for( TList::const_iterator it = list_.begin(); it != list_.end();  ++it )
	{
		arr.push_back( (*it)->toString() );
	}
	return arr;
}

template<class TList>
std::vector<std::wstring>		toStringArrayPtrListW( const TList& list_ )
{
	std::vector<std::wstring> arr;

	arr.reserve( list_.size() );
	for( TList::const_iterator it = list_.begin(); it != list_.end();  ++it )
	{
		arr.push_back( (*it)->toStringW() );
	}
	return arr;
}

/** This function creates an array of strings from an ImagingControl list pointer.
 * @param pList The list pointer you want to get the description strings.
 * @return When either pList is == 0 or pList is empty, then the array returned is empty.
 */
template<class TListPtr>
std::vector<std::string>		toStringArrayPtrListPtr( const TListPtr& pList )
{
	if( pList == 0 )		return std::vector<std::string>();
	return toStringArrayPtrList( *pList );
}

template<class TListPtr>
std::vector<std::wstring>		toStringArrayPtrListPtrW( const TListPtr& pList )
{
	if( pList == 0 )		return std::vector<std::wstring>();
	return toStringArrayPtrListW( *pList );
}

/** Presents the user with a choice of the specified range.
 * @return -1 when either the range is empty, or the user specified a wrong index for the range. Otherwise
 *				the index the user specified.
 */
template<class TIter>
int			presentUserChoice( TIter contBeg, TIter contEnd )
{
	if( contBeg == contEnd )
	{
		return -1;
	}

	int count = 0;
	for( ; contBeg != contEnd;  ++contBeg )
	{
		std::cout << "[" << count++ << "] " << *contBeg << std::endl;
		//std::cout << "[" << count++ << "] " << contBeg->toString() << std::endl;
	}

	std::cout << "Your choice : " << std::flush;

	int choice = -1;
	std::cin >> choice;
	if( !std::cin || choice < 0 || choice >= count )
	{
		return -1;
	}
	return choice;
}
template<class TIter>
int			presentUserChoiceW( TIter contBeg, TIter contEnd )
{
	if( contBeg == contEnd )
	{
		return -1;
	}

	int count = 0;
	for( ; contBeg != contEnd;  ++contBeg )
	{
		std::wcout << L"[" << count++ << L"] " << *contBeg << std::endl;
	}

	std::wcout << L"Your choice : " << std::flush;

	int choice = -1;
	std::cin >> choice;
	if( !std::cin || choice < 0 || choice >= count )
	{
		return -1;
	}
	return choice;
}


template<class TStringContainer>
int			presentUserChoice( const TStringContainer& cont )
{
	return presentUserChoice( cont.begin(), cont.end() );
}
template<class TStringContainer>
int			presentUserChoiceW( const TStringContainer& cont )
{
	return presentUserChoiceW( cont.begin(), cont.end() );
}

/** This convenience method loads a previously selected device and when a new device is selected, saves this setting.
 *
 * This method helps implementing a console application, by providing a 'last opened state' setting.
 * @param gr The Grabber in which the device is opened.
 * @param devStateFilename	The file to use as settings file.
 * @return true when a device was opened, false otherwise.
 */
inline bool	setupDeviceFromFile( _DSHOWLIB_NAMESPACE::Grabber& gr, 
								const std::string& devStateFilename = "lastSelectedDeviceState.xml" )
{
	if( gr.loadDeviceStateFromFile( devStateFilename ) )
	{
		std::cout << "Device opened from: <" << devStateFilename << ">." << std::endl;
	}
	else
	{
		std::cout << "File <" << devStateFilename << "> either not found, or device could not be opened from it." << std::endl;
	}
	if( !gr.showDevicePage() || !gr.isDevValid() )
	{
		return false;
	}
	gr.saveDeviceStateToFile( devStateFilename, true, true, false );
	return true;
}



#endif // CMDHELPER_H_INC_
