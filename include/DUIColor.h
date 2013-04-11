#ifndef __DUI_COLOR_H__
#define __DUI_COLOR_H__

#include "DUIBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API CARGB
{
public:
	CARGB();
	CARGB(BYTE r, BYTE g, BYTE b, BYTE a = 0);

	VOID SetColor(COLORREF clr);		//RGB(255, 0, 0) red =>0000FF
	VOID SetColor(LPCTSTR lpszColor);	//ARGB(FF-FF-FF-FF) "FF0000" red =>FF0000
	COLORREF GetColor() const;

	BOOL IsEmpty() const;

	BYTE GetA() const { return m_byteA; }
	BYTE GetG() const { return m_byteG; }
	BYTE GetR() const { return m_byteR; }
	BYTE GetB() const { return m_byteB; }

	BYTE& GetA()  { return m_byteA; }
	BYTE& GetG()  { return m_byteG; }
	BYTE& GetR()  { return m_byteR; }
	BYTE& GetB()  { return m_byteB; }

	VOID SetA(BYTE b)  { m_byteA = b; }
	VOID SetG(BYTE b)  { m_byteG = b; }
	VOID SetR(BYTE b)  { m_byteR = b; }
	VOID SetB(BYTE b)  { m_byteB = b; }

	VOID WriteToDIB4(LPBYTE lp) const;
	VOID WriteToDIB3(LPBYTE lp) const;
	
	LPBYTE ReadFromDIB4(LPBYTE lp);
	LPBYTE ReadFromDIB3(LPBYTE lp);

private:
	BYTE m_byteB;
	BYTE m_byteG;
	BYTE m_byteR;
	BYTE m_byteA;
};

class DUILIB_API CHLSColor
{
public:
	unsigned char red;
	unsigned char green;
	unsigned char blue;

	double lightness, saturation, hue;

	CHLSColor()
	{
	}
	
	CHLSColor(CARGB& clr)
	{
		red = clr.GetR();
		green = clr.GetG();
		blue = clr.GetB();
	}

	CHLSColor(double h, double l, double s)
	{
		hue		   = h;
		lightness  = l;
		saturation = s;

		CorrectHLS();
	}

	CHLSColor (BYTE r, BYTE g, BYTE b)
	{
		red   = r;
		green = g;
		blue  = b;
	}

	CHLSColor(COLORREF color)
	{
		red   = GetRValue(color);
		green = GetGValue(color);
		blue  = GetBValue(color);
	}
	
	void ToHLS(void);
	void ToRGB(void);

	void CorrectHLS();

	operator COLORREF() const;

protected:
	typedef enum
	{
		Red,
		Green,
		Blue
	};
};


DUI_END_NAMESPCE


#endif //__DUI_COLOR_H__