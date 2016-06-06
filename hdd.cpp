//------------------------------------------------------------------------------
/// @brief rotating disk-based storage devices (HDD)
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2016/05/22 Bernhard Egger created
///
/// @section license_section License
/// Copyright (c) 2016, Bernhard Egger
/// All rights reserved.
///
/// Redistribution and use in source and binary forms,  with or without modifi-
/// cation, are permitted provided that the following conditions are met:
///
/// - Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
/// - Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY  AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER  OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSE-
/// QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE
/// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
/// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT
/// LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY
/// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
/// DAMAGE.
//------------------------------------------------------------------------------

#include <cassert>
#include <limits>
#include <cmath>
#include <cstdlib>

#include <iostream>
#include <iomanip>

#include "hdd.h"
using namespace std;

//------------------------------------------------------------------------------
// HDD
//
HDD::HDD(uint32 surfaces, uint32 tracks_per_surface,
         uint32 sectors_innermost_track, uint32 sectors_outermost_track,
         uint32 rpm, uint32 sector_size,
         double seek_overhead, double seek_per_track,
         bool verbose)
  : _surfaces(surfaces), _rpm(rpm), _sector_size(sector_size),
    _seek_overhead(seek_overhead), _seek_per_track(seek_per_track),
    _verbose(verbose), _sectors_innermost_track(sectors_innermost_track)
{

  /* check validity */
  if (sectors_outermost_track <= sectors_innermost_track)
    cout << "Error: outermost track should contain more sectors than innermost" << endl;


  _sectors_diff = (double)(sectors_outermost_track - sectors_innermost_track) 
                  / (tracks_per_surface - 1);
 
  /* calculate total sector # */
  uint32 total_sector = 0;
  double track_sector = (double)_sectors_innermost_track;

  for (int i = 0; i < tracks_per_surface; i++)
    total_sector += (uint32)floor(track_sector + (_sectors_diff * i));
  
  total_sector *= _surfaces;

  _capacity = (total_sector/1000000000.0) * sector_size;
  _head_pos = 0; // head starts at track 0
  //
  // print info
  //
  cout.precision(3);
  cout << "HDD: " << endl
       << "  surfaces:                  " << _surfaces << endl
       << "  tracks/surface:            " << tracks_per_surface << endl
       << "  sect on innermost track:   " << sectors_innermost_track << endl
       << "  sect on outermost track:   " << sectors_outermost_track << endl
       << "  rpm:                       " << rpm << endl
       << "  sector size:               " << _sector_size << endl
       << "  number of sectors total:   " << total_sector << endl
       << "  capacity (GB):             " << _capacity << endl
       << endl;
}

HDD::~HDD(void)
{
  // TODO
}

double HDD::read(double ts, uint64 address, uint64 size)
{
  if (_verbose)
    cout << "HDD::read(" << ts << ", " << hex << address << ", " << hex << size << ")" << endl;
  // FIXME
  // what if size goes beyond disk size?
  uint32 sectors = size / _sector_size;

  if (decode(address, &_target_pos))
  {
    ts += seek_time(_head_pos, _target_pos.track) + wait_time() + read_time(sectors);
  }

  return ts;
}

double HDD::write(double ts, uint64 address, uint64 size)
{
  // TODO
  if (_verbose)
    cout << "HDD::write(" << ts << ", " << hex << address << ", " << hex << size << ")" << endl;
  // FIXME
  // what if size goes beyond disk size?
  uint32 sectors = size / _sector_size;

  if (decode(address, &_target_pos))
  {
    ts += seek_time(_head_pos, _target_pos.track) + wait_time() + read_time(sectors);
  }

  return ts;
}

double HDD::seek_time(uint32 from_track, uint32 to_track)
{
  if (from_track == to_track)
    return 0.0;

  return abs((double)to_track - (double)from_track) * _seek_per_track + _seek_overhead;
}

double HDD::wait_time(void)
{
  // average rotational latency = (1/2) * (1/RPM) * (60sec/1min)
  return ((double)1/2) * ((double)1/_rpm) * 60;
}

double HDD::read_time(uint64 sectors)
{
  // TODO
  double time = 0;
  uint32 read = 0;
  uint32 track_sector;
  uint32 max_access;

  HDD_Position curr_pos;
  curr_pos.surface = _target_pos.surface;
  curr_pos.track   = _target_pos.track;
  curr_pos.sector  = _target_pos.sector;

  while (1)
  {
    track_sector = (uint32) floor((double)_sectors_innermost_track + (_sectors_diff * curr_pos.track));
    max_access   = ((track_sector - (curr_pos.sector + 1)) * _surfaces) + (_surfaces - curr_pos.surface);
    
    // read until max_access in a single track
    for (read = 0; sectors > 0 && read < max_access; read++)
    {
      time += (1/(double)_rpm) * (1/(double)track_sector) * 60;
      sectors--;
    }
    
    if (sectors <= 0)
      break;

    curr_pos.surface = 0;
    curr_pos.track ++;
    curr_pos.sector = 0;
  
    // add wait_time everytime head changes track
    time += seek_time(curr_pos.track, curr_pos.track+1) + wait_time(); 
  }
  
  _head_pos = curr_pos.track;
  return time;
}

double HDD::write_time(uint64 sectors)
{
  double time = 0;
  uint32 read = 0;
  uint32 track_sector;
  uint32 max_access;

  HDD_Position curr_pos;
  curr_pos.surface = _target_pos.surface;
  curr_pos.track   = _target_pos.track;
  curr_pos.sector  = _target_pos.sector;

  while (1)
  {
    track_sector = (uint32) floor((double)_sectors_innermost_track + (_sectors_diff * curr_pos.track));
    max_access   = ((track_sector - (curr_pos.sector + 1)) * _surfaces) + (_surfaces - curr_pos.surface);
    
    // read until max_access in a single track
    for (read = 0; sectors > 0 && read < max_access; read++)
    {
      time += (1/(double)_rpm) * (1/(double)track_sector) * 60;
      sectors--;
    }
    
    if (sectors <= 0)
      break;

    curr_pos.surface = 0;
    curr_pos.track ++;
    curr_pos.sector = 0;
  
    // add wait_time everytime head changes track
    time += seek_time(curr_pos.track, curr_pos.track+1) + wait_time(); 
  }
  
  _head_pos = curr_pos.track;
  return time;
}

bool HDD::decode(uint64 address, HDD_Position *pos)
{
  // check address validity: 0 <= address < capacity
  if (address < 0 || address > (uint64)(_capacity * 1000000000.0))
    return false;

  uint32 block_index = address / _sector_size;
  uint32 surface_index = 0;
  uint32 track_index = 0;
  uint32 sector_index = 0;
  uint32 max_access = 0;
  uint32 total_sector = 0;
  uint32 track_sector = 0; // number of sectors per track (on 1 surface)
  
  /* dertermine track index */
  while (1)
  {
    track_sector = (uint32) floor((double)_sectors_innermost_track + (_sectors_diff * track_index));
    total_sector += track_sector * _surfaces;

    if (total_sector-1 >= block_index)
      break;

    track_index++;
  }

  /* determine surface index & sector index */
  // number of sectors on current track
  track_sector = (uint32) floor((double)_sectors_innermost_track + (_sectors_diff * track_index)); 
  // set total_sector to first block index on current track
  total_sector -= track_sector * _surfaces; 
  bool end_flag = true;

  while (sector_index < track_sector && total_sector < block_index)
  {
    for (surface_index = 0; surface_index < _surfaces; surface_index++)
    {
      if (total_sector >= block_index)
      {
        end_flag = false;
        break;
      }

      total_sector++;
    }

    if (end_flag)
    {
      surface_index = surface_index % _surfaces;
      sector_index++;
    }
  }
  
  pos->surface = surface_index;
  pos->track = track_index;
  pos->sector = sector_index;
  max_access = ((track_sector - (pos->sector + 1)) * _surfaces) + (_surfaces - pos->surface); 

  /* print info */
  if (_verbose)
  {
    cout << "HDD:decode(" << hex << address << ")" << endl
         << "  block index:    " << dec << block_index << endl
         << "  position:" << endl
         << "    surface:      " << pos -> surface << endl
         << "    track:        " << pos -> track << endl
         << "    sector:       " << pos -> sector<< endl
         << "    max. access:  " << max_access << endl
         << endl;
  }

  return true;
}


