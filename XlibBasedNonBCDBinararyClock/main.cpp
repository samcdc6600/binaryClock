#include <stdio.h>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <sstream>

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

constexpr unsigned int winWidth {91}, winBoarder_width {};
constexpr int numBitsHour {5}; // Not all of the variables on this line and the next two need to be global because they arn't used in more then.
constexpr int numBitsMinSec {6}; // One function. However since we are using some of them to calculate the window height and they are all.
constexpr int hGap {10}, vGap {10}, width {17}, height {17}; // Constants it was deemed acceptible make them all global since they are all related.
constexpr int textHeight {10};


bool getConfigurableParameters(const char * configPath, int & winX, int & winY);
int calcWinHeight();
bool init(const int winX, const int winY, const char * configPath, context & con, int & winHeight);
inline void display(context & con, const time_t time, const int winHeight);
inline void draw(context & con, const time_t time, const int winHeight);
inline void extractField(std::stringstream & date, std::stringstream & dateTime, const bool isStart, int count, const bool divider);
inline void extractTimeFields(const std::string time, int & hours, int & minutes, int & seconds);
inline void strNumPosToInt(const std::string & time, int & number, const int magnitude, const int iter);
inline void getBits(const int numBits, const int number, std::vector<bool> & ret);
inline void drawBits(context & con, const std::vector<bool> & bits, const int column, const int winHeight);


int main()
{
  const char * homeDir {getenv("HOME")}; // Get home directory
  std::stringstream configPath {};
  configPath<<homeDir<<"/.config/binClock.conf";
  int winX {}, winY {};
  if(getConfigurableParameters(configPath.str().c_str(), winX, winY))
    {
      context con;
      int winHeight {calcWinHeight()};
      if(init(winX, winY, configPath.str().c_str(), con, winHeight))
	{
	  time_t currentTime;
	  while(true)
	    {
	      time(&currentTime);	// Get current time
	      display(con, currentTime, winHeight);
	      std::this_thread::sleep_for(std::chrono::milliseconds(250));
	    }
	}
      XCloseDisplay(con.display);
    }
  
  return 0;
}


bool getConfigurableParameters(const char * configPath, int & winX, int & winY)
{
  bool ret {false};
  constexpr char spacer {':'}, endChar {';'};
  char skip {}, end {};
  std::ifstream in(configPath);
  if(in.is_open())
    {
      ret = true;
      in>>winX>>skip>>winY>>end;
      in.close();
      if(skip != spacer || end != endChar)
	{
	  std::cout<<"Error configuration file \""<<configPath<<"\" malformed!\nUsage:\tx:y; (where x & y are the coordinates of the top left corner"
	    " of the window, '"<<spacer<<"' act's as the field seperator and the last field must be followed by '"<<endChar<<"'.\n";
	  ret = false;
	}
    }
  else
    {
      std::cout<<"Unable to open file \""<<configPath<<"\". $HOME environment variable may not be set or file may not exist!\n";
    }  
  return ret;
}


int calcWinHeight()
{ // textHeight and vGap are multiplied by two the same multiplication is done in xdraw string (pleas clean this up and make all the code nice :) .)
  return (vGap * (numBitsMinSec +1)) + (height * numBitsMinSec) + textHeight*2 + vGap*2;
}



/*
  https://stackoverflow.com/questions/26017771/what-does-screen-number-and-display-number-mean-in-xlib
  "Display" in xlib / x11 protocol terminology is one single connection between client and X server.

  "Screen" is actual screen, but things get more complicated here. Each screen has its own root window ( and some more associated properties - 
  physical width/heights, DPI etc ). Because every window on the screen is child of that root window, you can't just move window from one screen to
  another (all child windows under X11 always clipped by parent). This is one of the reason multiple "screens" as in your question almost never used
  - most people have multiple monitors configured to be part of one X11 screen using Xinerama/RANDR extensions
  :(
*/
bool init(const int winX, const int winY, const char * configPath, context & con, int & winHeight)
{
  bool ret {false};
  con.display = XOpenDisplay(nullptr);
  
  if(!con.display)
    {
      std::cerr<< "Cannot to open con.display.";
      exit(1);
    }
  
  Screen * s = DefaultScreenOfDisplay(con.display);

  if(!(winX >= 0 && winX <= WidthOfScreen(s) - winWidth)) // To do subtract width of window from WidthOfScreen(s) (Also make sure that it should be >= and not >.)
    {
      std::cout<<"X ("<<winX<<") coordinate from configuration file \""<<configPath<<"\" out of range, where the allowable range is [0, "
	       <<WidthOfScreen(s) - winWidth<<"]\n";
    }
  else
    {
      if(!(winY >= 0 && winY <= HeightOfScreen(s) - winHeight))
	{
	  std::cout<<"Y ("<<winY<<") coordinate from configuration file \""<<configPath<<"\" out of range, where the allowable range is [0, "<<
	    HeightOfScreen(s) - winHeight<<"]\n";
	}
      else
	{
	  ret = true;
	  con.attribs.override_redirect = 1;//non bordered / decorated window.   
	  con.window = XCreateWindow( con.display, RootWindow(con.display, 0), winX, winY, winWidth, winHeight, winBoarder_width, CopyFromParent,
				      CopyFromParent, CopyFromParent, CWOverrideRedirect, &con.attribs );
  
	  XSetWindowBackground( con.display, con.window, 0x1900ff ); //0x84ffdc cool color
	  XClearWindow( con.display, con.window );
	  XMapWindow( con.display, con.window );

	  XGCValues values;
	  con.cmap = DefaultColormap(con.display, DefaultScreen(con.display));
	  con.gc = XCreateGC(con.display, con.window, 0, &values);

	  std::vector<std::string> colors {"Cyan", "Purple", "Blue", "Green", "Yellow", "Orange", "Red", "Dark Red"};

	  Status rc;
	  for(int iter {}; iter < colors.size(); ++iter)
	    {
	      rc = XAllocNamedColor(con.display, con.cmap, colors[iter].c_str(), con.xColors[iter], con.xColors[iter]);
	      if(rc == 0)
		{
		  std::cerr<<"XAllocNamedColor - failed to allocated '"<<colors[iter].c_str()<<"' color.\n";
		  exit(1);
		}
	    }
	}
    }
  return ret;
}


inline void display(context & con, const time_t time, const int winHeight)
{
  draw(con, time, winHeight);
  XFlush(con.display); //force x to flush it's buffers after we draw
}


inline void draw(context & con, const time_t time, const int winHeight)
{
  XClearWindow(con.display, con.window);//clear the window before we draw to it again
  std::stringstream dateTime, dateStart, dateEnd, sSTime;
  dateTime<<ctime(&time);
  extractField(dateStart, dateTime, true, 2, true);
  extractField(dateEnd, dateTime, false, 1, false);
  extractField(sSTime, dateTime, true, 1, true);
  extractField(dateEnd, dateTime, false, 1, true);
  int hours {}, minutes {}, seconds {};
  extractTimeFields(sSTime.str(), hours, minutes, seconds);
  std::vector<bool> bits;
  getBits(numBitsHour, hours, bits);
  drawBits(con, bits, 0, winHeight);//draw hour bit's in column 0
  bits.clear();
  getBits(numBitsMinSec, minutes, bits);
  drawBits(con, bits, 1, winHeight);//draw minutes bit's in column 1
  bits.clear();
  getBits(numBitsMinSec, seconds, bits);
  drawBits(con, bits, 2, winHeight);//draw seconds bit's in column 2
  XSetForeground(con.display, con.gc, con.cyan.pixel);//set forground colour
  XDrawString(con.display, con.window, con.gc, hGap , textHeight + vGap, dateStart.str().c_str(), dateStart.str().size());
  XDrawString(con.display, con.window, con.gc, hGap, textHeight*2 + vGap*2, dateEnd.str().c_str(), dateEnd.str().size());
}

    
inline void extractField(std::stringstream & date, std::stringstream & dateTime, const bool isStart, int count, const bool divider)
{
  std::string tmp {};  
  if(isStart)
    {
      dateTime>>tmp;
      date<<tmp;
      --count;
    }
  while(count--)
    {
      dateTime>>tmp;
      if(divider)
	date<<" | ";
      date<<tmp;
    }
}


inline void extractTimeFields(const std::string time, int & hours, int & minutes, int & seconds)
{
  int iter {};
  for(int magnitude {1}; time[iter] != ':' && time[iter] != '\0' && iter < time.size(); ++iter, magnitude *= 10)
    strNumPosToInt(time, hours, magnitude, iter);
  iter++;
  for(int magnitude {1}; time[iter] != ':' && time[iter] != '\0' && iter < time.size(); ++iter, magnitude *= 10)
    strNumPosToInt(time, minutes, magnitude, iter);
  iter++;
  for(int magnitude {1}; time[iter] != ':' && time[iter] != '\0' && iter < time.size(); ++iter, magnitude *= 10)
    strNumPosToInt(time, seconds, magnitude, iter);
}


inline void strNumPosToInt(const std::string & time, int & number, const int magnitude, const int iter)
{
  number *= magnitude;
  number += (time[iter] - '0');
}


inline void getBits(const int numBits, const int number, std::vector<bool> & ret)
{
  for(int iter {}; iter < numBits; ++iter)
    ret.push_back(((number>>iter)&1));
}


inline void drawBits(context & con, const std::vector<bool> & bits, const int column, const int winHeight)
{
  for(unsigned long rowIter {bits.size()}; rowIter > 0; --rowIter)
    {
      XSetForeground(con.display, con.gc, con.darkRed.pixel);
      if(bits[rowIter-1])
	XFillRectangle(con.display, con.window, con.gc, (hGap*(column+1)) + (width*column), winHeight - ((vGap*rowIter) + (height*rowIter)), width, height);
      else
	{
	    XSetForeground(con.display, con.gc, con.cyan.pixel);
	    XFillRectangle(con.display, con.window, con.gc, (hGap*(column+1)) + (width*column), winHeight - ((vGap*rowIter) + (height*rowIter)), width, height);
	}
    }
}
