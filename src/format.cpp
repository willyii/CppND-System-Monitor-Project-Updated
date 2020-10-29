#include "format.h"

#include <string>

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
  // hours
  int hours = seconds / (60 * 60);
  string sthour = to_string(hours);
  if (hours < 10) sthour = "0" + sthour;
  seconds = seconds % (60 * 60);

  // minutes
  int mins = seconds / (60);
  string stmin = to_string(mins);
  if (mins < 10) stmin = "0" + stmin;
  seconds = seconds % (60);

  // seconds
  string sts = to_string(seconds);
  if (seconds < 10) sts = "0" + sts;
  return sthour + ":" + stmin + ":" + sts;
}