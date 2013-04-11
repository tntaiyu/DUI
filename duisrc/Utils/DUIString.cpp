#include "DUIString.h"


CDUIString  stringReplace(const   CDUIString&   input,  
	const   CDUIString&   find,
	const   CDUIString&   replaceWith) 
{ 
	CDUIString   strOut(input); 
	int   curPos   =   0; 
	
	int   pos; 
	while((pos   =   strOut.find(find,   curPos))   !=   -1) 
	{ 
		strOut.replace(pos,   find.size(),   replaceWith); 
		curPos   =   pos   +   replaceWith.size(); 
	} 
	
	return   strOut; 
} 

