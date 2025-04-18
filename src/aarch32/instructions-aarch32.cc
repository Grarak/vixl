// Copyright 2017, VIXL authors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

extern "C" {
#include <stdint.h>
}

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "utils-vixl.h"
#include "aarch32/constants-aarch32.h"
#include "aarch32/instructions-aarch32.h"

namespace vixl {
namespace aarch32 {


std::ostream& operator<<(std::ostream& os, const Register reg) {
  switch (reg.GetCode()) {
    case 12:
      return os << "ip";
    case 13:
      return os << "sp";
    case 14:
      return os << "lr";
    case 15:
      return os << "pc";
    default:
      return os << "r" << reg.GetCode();
  }
}


std::ostream& PrintRegisterList(std::ostream& os,  // NOLINT(runtime/references)
                                uint32_t list) {
  os << "{";
  bool first = true;
  int code = 0;
  while (list != 0) {
    if ((list & 1) != 0) {
      if (first) {
        first = false;
      } else {
        os << ",";
      }
      os << Register(code);
    }
    list >>= 1;
    code++;
  }
  os << "}";
  return os;
}


std::ostream& operator<<(std::ostream& os, RegisterList registers) {
  return PrintRegisterList(os, registers.GetList());
}


QRegister VRegisterList::GetFirstAvailableQRegister() const {
  for (uint32_t i = 0; i < kNumberOfQRegisters; i++) {
    if (((list_ >> (i * 4)) & 0xf) == 0xf) return QRegister(i);
  }
  return QRegister();
}


DRegister VRegisterList::GetFirstAvailableDRegister() const {
  for (uint32_t i = 0; i < kMaxNumberOfDRegisters; i++) {
    if (((list_ >> (i * 2)) & 0x3) == 0x3) return DRegister(i);
  }
  return DRegister();
}


SRegister VRegisterList::GetFirstAvailableSRegister() const {
  for (uint32_t i = 0; i < kNumberOfSRegisters; i++) {
    if (((list_ >> i) & 0x1) != 0) return SRegister(i);
  }
  return SRegister();
}


std::ostream& operator<<(std::ostream& os, SRegisterList reglist) {
  SRegister first = reglist.GetFirstSRegister();
  SRegister last = reglist.GetLastSRegister();
  if (first.Is(last))
    os << "{" << first << "}";
  else
    os << "{" << first << "-" << last << "}";
  return os;
}


std::ostream& operator<<(std::ostream& os, DRegisterList reglist) {
  DRegister first = reglist.GetFirstDRegister();
  DRegister last = reglist.GetLastDRegister();
  if (first.Is(last))
    os << "{" << first << "}";
  else
    os << "{" << first << "-" << last << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, NeonRegisterList nreglist) {
  DRegister first = nreglist.GetFirstDRegister();
  int increment = nreglist.IsSingleSpaced() ? 1 : 2;
  int count =
      nreglist.GetLastDRegister().GetCode() - first.GetCode() + increment;
  if (count < 0) count += kMaxNumberOfDRegisters;
  os << "{";
  bool first_displayed = false;
  for (;;) {
    if (first_displayed) {
      os << ",";
    } else {
      first_displayed = true;
    }
    os << first;
    if (nreglist.IsTransferOneLane()) {
      os << "[" << nreglist.GetTransferLane() << "]";
    } else if (nreglist.IsTransferAllLanes()) {
      os << "[]";
    }
    count -= increment;
    if (count <= 0) break;
    unsigned next = first.GetCode() + increment;
    if (next >= kMaxNumberOfDRegisters) next -= kMaxNumberOfDRegisters;
    first = DRegister(next);
  }
  os << "}";
  return os;
}


const char* BankedRegister::GetName() const {
  switch (reg_) {
    case R8_usr:
      return "R8_usr";
    case R9_usr:
      return "R9_usr";
    case R10_usr:
      return "R10_usr";
    case R11_usr:
      return "R11_usr";
    case R12_usr:
      return "R12_usr";
    case SP_usr:
      return "SP_usr";
    case LR_usr:
      return "LR_usr";
    case R8_fiq:
      return "R8_fiq";
    case R9_fiq:
      return "R9_fiq";
    case R10_fiq:
      return "R10_fiq";
    case R11_fiq:
      return "R11_fiq";
    case R12_fiq:
      return "R12_fiq";
    case SP_fiq:
      return "SP_fiq";
    case LR_fiq:
      return "LR_fiq";
    case LR_irq:
      return "LR_irq";
    case SP_irq:
      return "SP_irq";
    case LR_svc:
      return "LR_svc";
    case SP_svc:
      return "SP_svc";
    case LR_abt:
      return "LR_abt";
    case SP_abt:
      return "SP_abt";
    case LR_und:
      return "LR_und";
    case SP_und:
      return "SP_und";
    case LR_mon:
      return "LR_mon";
    case SP_mon:
      return "SP_mon";
    case ELR_hyp:
      return "ELR_hyp";
    case SP_hyp:
      return "SP_hyp";
    case SPSR_fiq:
      return "SPSR_fiq";
    case SPSR_irq:
      return "SPSR_irq";
    case SPSR_svc:
      return "SPSR_svc";
    case SPSR_abt:
      return "SPSR_abt";
    case SPSR_und:
      return "SPSR_und";
    case SPSR_mon:
      return "SPSR_mon";
    case SPSR_hyp:
      return "SPSR_hyp";
  }
  VIXL_UNREACHABLE();
  return "??";
}


const char* EncodingSize::GetName() const {
  switch (size_) {
    case Best:
    case Narrow:
      return "";
    case Wide:
      return ".w";
  }
  VIXL_UNREACHABLE();
  return "??";
}


const char* InterruptFlags::GetName() const {
  switch (type_) {
    case F:
      return "f";
    case I:
      return "i";
    case IF:
      return "if";
    case A:
      return "a";
    case AF:
      return "af";
    case AI:
      return "ai";
    case AIF:
      return "aif";
  }
  VIXL_ASSERT(type_ == 0);
  return "";
}


const char* Endianness::GetName() const {
  switch (type_) {
    case LE:
      return "le";
    case BE:
      return "be";
  }
  VIXL_UNREACHABLE();
  return "??";
}


// Constructor used for disassembly.
ImmediateShiftOperand::ImmediateShiftOperand(int shift_value, int amount_value)
    : Shift(shift_value) {
  switch (shift_value) {
    case LSL:
      amount_ = amount_value;
      break;
    case LSR:
    case ASR:
      amount_ = (amount_value == 0) ? 32 : amount_value;
      break;
    case ROR:
      amount_ = amount_value;
      if (amount_value == 0) SetType(RRX);
      break;
    default:
      VIXL_UNREACHABLE();
      SetType(LSL);
      amount_ = 0;
      break;
  }
}


ImmediateT32::ImmediateT32(uint32_t imm) {
  // 00000000 00000000 00000000 abcdefgh
  if ((imm & ~0xff) == 0) {
    SetEncodingValue(imm);
    return;
  }
  if ((imm >> 16) == (imm & 0xffff)) {
    if ((imm & 0xff00) == 0) {
      // 00000000 abcdefgh 00000000 abcdefgh
      SetEncodingValue((imm & 0xff) | (0x1 << 8));
      return;
    }
    if ((imm & 0xff) == 0) {
      // abcdefgh 00000000 abcdefgh 00000000
      SetEncodingValue(((imm >> 8) & 0xff) | (0x2 << 8));
      return;
    }
    if (((imm >> 8) & 0xff) == (imm & 0xff)) {
      // abcdefgh abcdefgh abcdefgh abcdefgh
      SetEncodingValue((imm & 0xff) | (0x3 << 8));
      return;
    }
  }
  for (int shift = 0; shift < 24; shift++) {
    uint32_t imm8 = imm >> (24 - shift);
    uint32_t overflow = imm << (8 + shift);
    if ((imm8 <= 0xff) && ((imm8 & 0x80) != 0) && (overflow == 0)) {
      SetEncodingValue(((shift + 8) << 7) | (imm8 & 0x7F));
      return;
    }
  }
}


bool ImmediateT32::IsImmediateT32(uint32_t imm) {
  /* abcdefgh abcdefgh abcdefgh abcdefgh */
  if (AllBytesMatch(imm)) return true;
  /* 00000000 abcdefgh 00000000 abcdefgh */
  /* abcdefgh 00000000 abcdefgh 00000000 */
  if (AllHalfwordsMatch(imm) &&
      (((imm & 0xff00) == 0) || ((imm & 0xff) == 0))) {
    return true;
  }
  /* isolate least-significant set bit */
  uint32_t lsb = imm & UnsignedNegate(imm);
  /* if imm is less than lsb*256 then it fits, but instead we test imm/256 to
  * avoid overflow (underflow is always a successful case) */
  return ((imm >> 8) < lsb);
}


uint32_t ImmediateT32::Decode(uint32_t value) {
  uint32_t base = value & 0xff;
  switch (value >> 8) {
    case 0:
      return base;
    case 1:
      return base | (base << 16);
    case 2:
      return (base << 8) | (base << 24);
    case 3:
      return base | (base << 8) | (base << 16) | (base << 24);
    default:
      base |= 0x80;
      return base << (32 - (value >> 7));
  }
}


ImmediateA32::ImmediateA32(uint32_t imm) {
  // Deal with rot = 0 first to avoid undefined shift by 32.
  if (imm <= 0xff) {
    SetEncodingValue(imm);
    return;
  }
  for (int rot = 2; rot < 32; rot += 2) {
    uint32_t imm8 = (imm << rot) | (imm >> (32 - rot));
    if (imm8 <= 0xff) {
      SetEncodingValue((rot << 7) | imm8);
      return;
    }
  }
}


bool ImmediateA32::IsImmediateA32(uint32_t imm) {
  /* fast-out */
  if (imm < 256) return true;
  /* avoid getting confused by wrapped-around bytes (this transform has no
   * effect on pass/fail results) */
  if (imm & 0xff000000) imm = static_cast<uint32_t>(RotateRight(imm, 16, 32));
  /* copy odd-numbered set bits into even-numbered bits immediately below, so
   * that the least-significant set bit is always an even bit */
  imm = imm | ((imm >> 1) & 0x55555555);
  /* isolate least-significant set bit (always even) */
  uint32_t lsb = imm & UnsignedNegate(imm);
  /* if imm is less than lsb*256 then it fits, but instead we test imm/256 to
   * avoid overflow (underflow is always a successful case) */
  return ((imm >> 8) < lsb);
}


uint32_t ImmediateA32::Decode(uint32_t value) {
  int rotation = (value >> 8) * 2;
  VIXL_ASSERT(rotation >= 0);
  VIXL_ASSERT(rotation <= 30);
  value &= 0xff;
  if (rotation == 0) return value;
  return (value >> rotation) | (value << (32 - rotation));
}


uint32_t TypeEncodingValue(Shift shift) {
  return shift.IsRRX() ? kRRXEncodedValue : shift.GetValue();
}


uint32_t AmountEncodingValue(Shift shift, uint32_t amount) {
  switch (shift.GetType()) {
    case LSL:
    case ROR:
      return amount;
    case LSR:
    case ASR:
      return amount % 32;
    case RRX:
      return 0;
  }
  return 0;
}

}  // namespace aarch32
}  // namespace vixl
