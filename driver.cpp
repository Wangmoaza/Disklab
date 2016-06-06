#include <cassert>
#include <cstdlib>
#include <iostream>

#include "disk.h"
#include "hdd.h"
using namespace std;

int main(void)
{
  uint32 surfaces, tracks_per_surface, sectors_innermost, sectors_outermost,
         rpm, bytes_per_sector;
  double seek_overhead, seek_per_track;
  bool   verbose;

  HDD *hdd;
  double t;
  char rw;
  uint64 address, length;

  //
  // read HDD parameters
  //
  cin >> surfaces;
  cin >> tracks_per_surface;
  cin >> sectors_innermost;
  cin >> sectors_outermost;
  cin >> rpm;
  cin >> bytes_per_sector;
  cin >> seek_overhead;
  cin >> seek_per_track;
  cin >> verbose;

  if (!cin.good()) {
    cout << "Error reading HDD parameters from stdin" << endl
         << endl;
    return EXIT_FAILURE;
  }


  //
  // create new instance of HDD
  //
  hdd = new HDD(
      surfaces, tracks_per_surface,
      sectors_innermost, sectors_outermost,
      rpm, bytes_per_sector,
      seek_overhead, seek_per_track,
      verbose);

  //
  // standard tests
  //
  cout.precision(6);
  t = hdd->seek_time(0, tracks_per_surface/2);
  cout << "avg. seek time:    " << dec << fixed << t << endl;

  t = hdd->seek_time(0, 1);
  cout << "seek 1 track:      " << dec << fixed << t << endl;

  t = hdd->wait_time();
  cout << "avg. rot. latency: " << dec << fixed << t << endl;

  t = hdd->read_time(1);
  cout << "read 1 sector:     " << dec << fixed << t << endl;

  t = hdd->write_time(1);
  cout << "write 1 sector:    " << dec << fixed << t << endl;

  cout << endl << endl;


  //
  // process requests from input file
  //
  cin >> t >> rw >> address >> length;

  while (cin.good()) {
    cout.precision(6);
    switch (rw) {
      case 'r': cout << "read"; break;
      case 'w': cout << "write"; break;
      default : cout << "error in input trace";
    }

    cout << "(" << t << ", " << address << ", " << length << ") = ";
    cout.flush();

    switch (rw) {
      case 'r': t = hdd->read(t, address, length); break;
      case 'w': t = hdd->write(t, address, length); break;
    }
    cout.precision(6);
    cout << t << endl;

    cin >> t >> rw >> address >> length;
  }

  delete hdd;

  return EXIT_SUCCESS;
}

