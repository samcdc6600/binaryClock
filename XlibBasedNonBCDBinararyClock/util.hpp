#ifndef UTIL_HPP_
#define UTIL_HPP_


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
  Window window;
  XSetWindowAttributes attribs;
  Colormap cmap;
  XColor cyan, purple, blue, green, yellow, orange, red, darkRed;
  std::vector<XColor *> xColors {&cyan, &purple, &blue, &green, &yellow, &orange, &red, &darkRed};
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
  void checkRangeBg(const unsigned short c) const
  {
    if(c < backgroundColorRangeMin || c > backgroundColorRangeMax)
      {
	std::cerr<<"Error: in checkRangeBg - called from Color(), supplied background color or alpha value ("<<c<<") "
	  "is out of range! Where range is ["<<backgroundColorRangeMin<<","<<backgroundColorRangeMax<<"].\n";
	exit(error_values::COLOR_RANGE);
      }
  }

  void checkRange(const unsigned short c) const
  {
    if(c < colorRangeMin || c > colorRangeMax)
      {
	std::cerr<<"Error: in checkRange - called from Color(), supplied color value ("<<c<<") is out of range! Where"
	  " range is ["<<backgroundColorRangeMin<<","<<backgroundColorRangeMax<<"].\n";
	exit(error_values::COLOR_RANGE);
      }
  }

  void init()
  { /* We don't do this in the constructor because this class is somtimes just used to store the integer values and
       not for setting the actual color. */
    text.red = backgroundRed;		text.green = backgroundGreen;		text.blue = backgroundBlue;
    positiveBit.red = positiveBitRed;	positiveBit.green = positiveBitGreen;	positiveBit.blue = positiveBitBlue;
    negativeBit.red = negativeBitRed;	negativeBit.green = negativeBitGreen;	negativeBit.blue = negativeBitBlue;
  }

  void setBackground(XSetWindowAttributes attr) const
  {
    attr.background_pixel = backgroundColor;
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

public:
  Color(const unsigned short a,
        const unsigned short bGR,       const unsigned short bGG,       const unsigned short bGB,
        const unsigned short tR,        const unsigned short tG,        const unsigned short tB,
        const unsigned short pBR,       const unsigned short pBG,       const unsigned short pBB,
        const unsigned short nBR,       const unsigned short nBG,       const unsigned short nBB) :
    // alpha byte (msb) + red byte  + green byte + blue byte = 4 bytes
    backgroundColor ((a + backgroundCompSize * 3) +
		     (bGR + backgroundCompSize * 2) +
		     (bGG + backgroundCompSize * 1) +
		     (bGB + backgroundCompSize * 0)),
    alpha (a),
    backgroundRed (bGR),        backgroundGreen (bGG),  backgroundBlue (bGB),
    textRed (tR),               textGreen (tG),         textBlue (tB),
    positiveBitRed (pBR),       positiveBitGreen (pBG), positiveBitBlue (pBB),
    negativeBitRed (nBR),       negativeBitGreen (nBG), negativeBitBlue (nBB)
  {}
};


#endif
