//------------------------------------------------------------------------------
/// @brief disk-based storage devices
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

#ifndef __CA_DISK_H__
#define __CA_DISK_H__

//------------------------------------------------------------------------------
// a few useful type definitions
typedef unsigned long long uint64;        ///< 64-bit unsigned int
typedef          long long  int64;        ///< 64-bit signed int
typedef unsigned int       uint32;        ///< 32-bit unsigned int
typedef          int        int32;        ///< 32-bit signed int

//------------------------------------------------------------------------------
/// @brief base class for disk-based storage devices
///
/// Disk provides an abstract base class for all disk-based storage devices
/// (rotating HDDs, SDD, flash-disks, etc).
///
class Disk {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    Disk(void) {};

    /// @brief destructor
    virtual ~Disk(void) {};

    /// @}


    /// @name access methods
    /// @{

    /// @brief read @a size bytes from @a adr
    /// @param time time of the event
    /// @param adr starting address (in bytes) of data to read
    /// @param size number of bytes to read
    /// @retval time when the access ends (ts + latency of access)
    virtual double read(double time, uint64 adr, uint64 size) = 0;

    /// @brief write @a size bytes to @a adr
    /// @param time time of the event
    /// @param adr starting address (in bytes) of data to write
    /// @param size number of bytes to write
    /// @retval time when the access ends (ts + latency of access)
    virtual double write(double time, uint64 adr, uint64 size) = 0;

    /// @}
};

#endif // __CA_DISK_H__
