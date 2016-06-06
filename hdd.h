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

#ifndef __CA_HDD_H__
#define __CA_HDD_H__

#include "disk.h"
using namespace std;

///@brief struct encoding a byte position on the disk as a surface/track/sector 
///       triple.
typedef struct _hdd_pos {
  uint32 surface;                   ///< surface
  uint32 track;                     ///< track
  uint32 sector;                    ///< sector
  uint32 max_access;                ///< how many sectors can be accessed conse-
                                    ///< cutively until the end of this track
} HDD_Position;

//------------------------------------------------------------------------------
/// @brief rotating disk-based storage devices (HDD)
///
/// The HDD class implements rotating disks.
///
class HDD : public Disk {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    HDD(uint32 surfaces, uint32 tracks_per_surface,
        uint32 sectors_innermost_track, uint32 sectors_outermost_track,
        uint32 rpm, uint32 sector_size,
        double seek_overhead, double seek_per_track,
        bool verbose=false);

    /// @brief destructor
    virtual ~HDD(void);

    /// @}


    /// @name access methods
    /// @{

    /// @brief read @a size bytes from @a address
    /// @param ts timestamp of the event
    /// @param address starting address (in bytes) of data to read
    /// @param size number of bytes to read
    /// @retval time when the access ends (ts + latency of access)
    virtual double read(double ts, uint64 address, uint64 size);

    /// @brief write @a size bytes to @a address
    /// @param ts timestamp of the event
    /// @param address starting address (in bytes) of data to write
    /// @param size number of bytes to write
    /// @retval time when the access ends (ts + latency of access)
    virtual double write(double ts, uint64 address, uint64 size);

    /// @}


    /// @name access latencies
    /// @{

    /// @brief seek time to move the head from @from_track to @to_track
    double seek_time(uint32 from_track, uint32 to_track);

    /// @brief average rotational latency
    double wait_time(void);

    /// @brief time to read @sectors sectors
    double read_time(uint64 sectors);

    /// @brief time to write @sectors sectors
    double write_time(uint64 sectors);

    /// @}


  protected:
    uint32 _surfaces;               ///< number of surfaces
    bool   _verbose;                ///< toggle verbose output
    uint32 _head_pos;               ///< current position (track) of r/w heads.
    uint32 _rpm;                    ///< rotations per minute
    uint32 _sector_size;            ///< number of bytes per sector
    double _seek_overhead;          ///< seek overhead
    double _seek_per_track;         ///< seek time per track the head is moved
    uint32 _sectors_innermost_track;///< number of sectors in innermost track
    double _sectors_diff;           ///< sector number difference between tracks
    double _capacity;               ///< capacity of disk (GB)
    HDD_Position _target_pos;          ///< block position of desired address
    // TODO add more fields as necessary


    /// @brief translate a byte address into a position on the HDD
    /// @param address byte address
    /// @param pos (output) pointer to result
    /// @retval true if translation was successful, false otherwise
    bool   decode(uint64 address, HDD_Position *pos);
    

    // TODO
    // add more protected methods as necessary
};

#endif // __CA_HDD_H__
