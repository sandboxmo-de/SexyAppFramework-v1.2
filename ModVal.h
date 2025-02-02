#ifndef __MODVAL_H__
#define __MODVAL_H__

/*
 This module allows for dynamic modification of integer and floating-point 
 constants at run-time.

 Place the M() around values you want to allow to be changed at run-time. 
 When you want to modify a value, simply change the value in the source
 code file and call ReparseModValues() within your program to re-read the
 source files, checking for changes in constants.  Your program will need
 to contain some trigger such as a key combination to trigger the 
 ReparseModValues() call.

 Example:
	x = x + M(2.1);

 Caveats:
	This module determines which files to parse through (during
	ReparseModValues()) at run-time, so if a M() macro has not yet been
	executed within a particular source file, its value will not be 
	updated.

 Performance:
	There a small setup cost the first time each M() value is accessed
	after program startup and reparsing, but after that there is just the 
	tiny overhead of a function call and a few vector dereferences.

 */

namespace Sexy
{

#if defined(SEXY_DISABLE_MODVAL) || defined(RELEASEFINAL)
#define M(val)  (val)
#define M1(val) (val)
#define M2(val) (val)
#define M3(val) (val)
#define M4(val) (val)
#define M5(val) (val)
#define M6(val) (val)
#define M7(val) (val)
#define M8(val) (val)
#define M9(val) (val)
#else
#define M(val) ModVal(0, "M_"__FILE__, __LINE__, (val))
#define M1(val) ModVal(1, "M_"__FILE__, __LINE__, (val))
#define M2(val) ModVal(2, "M_"__FILE__, __LINE__, (val))
#define M3(val) ModVal(3, "M_"__FILE__, __LINE__, (val))
#define M4(val) ModVal(4, "M_"__FILE__, __LINE__, (val))
#define M5(val) ModVal(5, "M_"__FILE__, __LINE__, (val))
#define M6(val) ModVal(6, "M_"__FILE__, __LINE__, (val))
#define M7(val) ModVal(7, "M_"__FILE__, __LINE__, (val))
#define M8(val) ModVal(8, "M_"__FILE__, __LINE__, (val))
#define M9(val) ModVal(9, "M_"__FILE__, __LINE__, (val))
#endif

int				ModVal(int theAreaNum, const char* theFileName, int theLineNum, int theInt);
double			ModVal(int theAreaNum, const char* theFileName, int theLineNum, double theDouble);
float			ModVal(int theAreaNum, const char* theFileName, int theLineNum, float theFloat);
bool			ReparseModValues();

}

#endif //__MODVAL_H__