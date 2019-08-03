#!/bin/sh
if test "$1" != "-c" # Don't use config file in test (use command line args instead.)
then
    ./binclk 50 50 255 128 255 128 65535 0 0 0 65535 0 0 0 65535
else
    ./binclk
fi
