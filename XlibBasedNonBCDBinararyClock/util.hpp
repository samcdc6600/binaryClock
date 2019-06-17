#ifndef UTIL_HPP_
#define UTIL_HPP_


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
// TMP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <bitset>
// TMP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

namespace time_constants
{
  constexpr int numBitsHour {5};
  constexpr int numBitsMinSec {6};
}


namespace error_values
{
  enum
    {
      XLIB_INIT = INT_MIN,
      COLOR_RANGE
    };
}


namespace tunables
{
  constexpr unsigned int winWidth {91}, winBoarder_width {};
  constexpr int hGap {10}, vGap {10}, width {17}, height {17};
  constexpr int textHeight {10};
}


struct context
{
  Display *display = NULL;
  XVisualInfo vinfo;
  Window window;
  XSetWindowAttributes attribs;
  Colormap cmap;
  GC gc;
};


class Color
{
private: /* Ranges are inclusive. The red green and blue members of XColor are of type unsigned short, scaled from 0 
            to 65535 inclusive. */
  const size_t backgroundColorRangeMin {0}, backgroundColorRangeMax {255}, colorRangeMin {0}, colorRangeMax {65535};
  XColor text, positiveBit, negativeBit;
  // Used to store background color constructed from alpha, backgroundRed, backgroundGreen and backgroundBlue.
  const int backgroundCompSize {8}, backgroundColor {};

public:
  const int alpha {},
    backgroundRed {},   backgroundGreen {},     backgroundBlue {},
    textRed {},         textGreen {},           textBlue {},
    positiveBitRed {},  positiveBitGreen {},    positiveBitBlue {},
    negativeBitRed {},  negativeBitGreen {},    negativeBitBlue {};

private:
  unsigned short checkRangeBg(const unsigned short c) const
  {
    if(c < backgroundColorRangeMin || c > backgroundColorRangeMax)
      {
	std::cerr<<"Error: in checkRangeBg - called from Color(), supplied background color or alpha value ("<<c<<") "
	  "is out of range! Where range is ["<<backgroundColorRangeMin<<","<<backgroundColorRangeMax<<"].\n";
	exit(error_values::COLOR_RANGE);
      }
    return c;
  }

  unsigned short checkRange(const unsigned short c) const
  {
    if(c < colorRangeMin || c > colorRangeMax)
      {
	std::cerr<<"Error: in checkRange - called from Color(), supplied color value ("<<c<<") is out of range! Where"
	  " range is ["<<colorRangeMin<<","<<colorRangeMax<<"].\n";
	exit(error_values::COLOR_RANGE);
      }
    return c;
  }

public:
  Color(const unsigned short a,
        const unsigned short bGR,       const unsigned short bGG,       const unsigned short bGB,
        const unsigned short tR,        const unsigned short tG,        const unsigned short tB,
        const unsigned short pBR,       const unsigned short pBG,       const unsigned short pBB,
        const unsigned short nBR,       const unsigned short nBG,       const unsigned short nBB) :
    // alpha byte (msb) + red byte  + green byte + blue byte = 4 bytes
    backgroundColor ((checkRangeBg(a)<<backgroundCompSize<<backgroundCompSize<<backgroundCompSize) +
		     (checkRangeBg(bGR)<<backgroundCompSize<<backgroundCompSize) +
		     (checkRangeBg(bGG)<<backgroundCompSize) +
		     (checkRangeBg(bGB))),
    alpha (a),
    backgroundRed (checkRangeBg(bGR)),	backgroundGreen (checkRangeBg(bGG)),	backgroundBlue (checkRangeBg(bGB)),
    textRed (checkRange(tR)),		textGreen (checkRange(tG)),		textBlue (checkRange(tB)),
    positiveBitRed (checkRange(pBR)),	positiveBitGreen (checkRange(pBG)),	positiveBitBlue (checkRange(pBB)),
    negativeBitRed (checkRange(nBR)),	negativeBitGreen (checkRange(nBG)),	negativeBitBlue (checkRange(nBB))
  {}

  /*  void init(Display *display = NULL, Colormap cmap)
  { /* We don't do this in the constructor because this class is somtimes just used to store the integer values and
       not for setting the actual color. */
  /*    text.red = backgroundRed;		text.green = backgroundGreen;		text.blue = backgroundBlue;
    positiveBit.red = positiveBitRed;	positiveBit.green = positiveBitGreen;	positiveBit.blue = positiveBitBlue;
    negativeBit.red = negativeBitRed;	negativeBit.green = negativeBitGreen;	negativeBit.blue = negativeBitBlue;
    XAllocColor(display, cmap, & text);
    XAllocColor(display, cmap, & positiveBit);
    XAllocColor(display, cmap, & negativeBit);
    }*/

  void setBackground(XSetWindowAttributes attr) const
  {
    attr.background_pixel = backgroundColor;
    std::cout<<"backgroundColor = "<<std::bitset<32>(backgroundColor)<<std::endl;
  }

  void setText(Display * display, const GC gc) const
  {
    XSetForeground(display, gc, text.pixel);
  }

  void setPositiveBit(Display * display, const GC gc) const
  {
    XSetForeground(display, gc, positiveBit.pixel);
  }

  void setNegativeBit(Display * display, const GC gc) const
  {
    XSetForeground(display, gc, negativeBit.pixel);
  }
};


#endif
