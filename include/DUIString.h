#ifndef __DUI_STRING_H__
#define __DUI_STRING_H__

#include <iosfwd>
#include <xstring>
typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> >			StdStringA; 
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >	StdStringW; 

#ifdef _UNICODE
#define CDUIString StdStringW
#else
#define CDUIString StdStringA
#endif //_UNICODE



CDUIString  stringReplace(const   CDUIString&   input,  
	const   CDUIString&   find,
	const   CDUIString&   replaceWith);


#endif //__DUI_STRING_H__