#ifndef UTIL_HPP_
#define UTIL_HPP_


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

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
private: /* Ranges are inclusive. The red green and blue members of XColor are
	    of type unsigned short, scaled from 0 to 65535 inclusive. */
  const size_t backgroundColorRangeMin {0}, backgroundColorRangeMax {255},
    colorRangeMin {0}, colorRangeMax {65535};
  XColor text, positiveBit, negativeBit;
  /* Used to store background color constructed from alpha, backgroundRed,
     backgroundGreen and backgroundBlue. */
  const int backgroundCompSize {8}, backgroundColor {};

public:
  const int alpha {},
    backgroundRed {},   backgroundGreen {},     backgroundBlue {},
    textRed {},         textGreen {},           textBlue {},
    positiveBitRed {},  positiveBitGreen {},    positiveBitBlue {},
    negativeBitRed {},  negativeBitGreen {},    negativeBitBlue {};

private:
  unsigned short checkRangeBg(const int c) const
  {
    if(c < int(backgroundColorRangeMin) || c > int(backgroundColorRangeMax))
      {
	std::cerr<<"Error: in checkRangeBg - called from Color(), supplied "
	  "background color or alpha value ("<<c<<") is out of range! Where "
	  "range is ["<<backgroundColorRangeMin<<","
		 <<backgroundColorRangeMax<<"].\n";
	exit(error_values::COLOR_RANGE);
      }
    return (unsigned short)(c);
  }

  unsigned short checkRange(const int c) const
  {
    if(c < (int)colorRangeMin || c > (int)colorRangeMax)
      {
	std::cerr<<"Error: in checkRange - called from Color(), supplied color "
	  "value ("<<c<<") is out of range! Where"
	  " range is ["<<colorRangeMin<<","<<colorRangeMax<<"].\n";
	exit(error_values::COLOR_RANGE);
      }
    return (unsigned short)(c);
  }

public:
  Color(const int a,
        const int bgR,	const int bgG,	const int bgB,
	const int tR,	const int tG,	const int tB,
        const int pbR,	const int pbG,	const int pbB,
	const int nbR,	const int nbG,	const int nbB) :
    // alpha byte (msb) + red byte  + green byte + blue byte = 4 bytes
    backgroundColor ((checkRangeBg(a)<<backgroundCompSize<<backgroundCompSize
		      <<backgroundCompSize) +
		     (checkRangeBg(bgR)<<backgroundCompSize<<backgroundCompSize)
		     + (checkRangeBg(bgG)<<backgroundCompSize)
		     + (checkRangeBg(bgB))),
    alpha (a),
    backgroundRed (checkRangeBg(bgR)),	backgroundGreen (checkRangeBg(bgG)),
    backgroundBlue (checkRangeBg(bgB)),	textRed (checkRange(tR)),
    textGreen (checkRange(tG)),		textBlue (checkRange(tB)),
    positiveBitRed (checkRange(pbR)),	positiveBitGreen (checkRange(pbG)),
    positiveBitBlue (checkRange(pbB)),	negativeBitRed (checkRange(nbR)),
    negativeBitGreen (checkRange(nbG)),	negativeBitBlue (checkRange(nbB))
  {}

  /* These init functions are required because we don't do this in the
     constructor as this class is somtimes used just for the purpose of storing
     color values and not actually setting them */
  // Sets background color, should be called before XCreateWindow().
  void initBackground(XSetWindowAttributes * attr) const
  {
    attr->background_pixel = backgroundColor;
  }

  // Init() should be called after XCreateWindow().
  void init(Display * display, Colormap cmap)//, XSetWindowAttributes attr)
  {
    text.red = textRed;		text.green = textGreen;
    text.blue = textBlue;
    positiveBit.red = positiveBitRed;	positiveBit.green = positiveBitGreen;
    positiveBit.blue = positiveBitBlue;
    negativeBit.red = negativeBitRed;	negativeBit.green = negativeBitGreen;
    negativeBit.blue = negativeBitBlue;
    negativeBit.flags = DoRed | DoGreen;
    negativeBit.flags &= !DoBlue;
    XAllocColor(display, cmap, & text);
    XAllocColor(display, cmap, & positiveBit);
    XAllocColor(display, cmap, & negativeBit);
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
