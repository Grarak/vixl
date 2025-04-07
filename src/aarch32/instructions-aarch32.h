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

#ifndef VIXL_AARCH32_INSTRUCTIONS_AARCH32_H_
#define VIXL_AARCH32_INSTRUCTIONS_AARCH32_H_

extern "C" {
#include <stdint.h>
}

#include <algorithm>
#include <ostream>

#include "code-buffer-vixl.h"
#include "utils-vixl.h"
#include "aarch32/constants-aarch32.h"
#include "aarch32/wrapper-aarch32.h"

#if defined(__arm__) && !defined(__SOFTFP__)
#define HARDFLOAT __attribute__((noinline, pcs("aapcs-vfp")))
#elif defined(_MSC_VER)
#define HARDFLOAT __declspec(noinline)
#else
#define HARDFLOAT __attribute__((noinline))
#endif

class SRegister;
class DRegister;
class QRegister;
class MemOperand;
class Operand;

namespace vixl {
namespace aarch32 {

class SOperand;
class DOperand;
class QOperand;
class AlignedMemOperand;

std::ostream& operator<<(std::ostream& os, const Register reg);

const RegisterOrAPSR_nzcv APSR_nzcv(kPcCode);

inline std::ostream& operator<<(std::ostream& os,
                                const RegisterOrAPSR_nzcv reg) {
  if (reg.IsAPSR_nzcv()) return os << "APSR_nzcv";
  return os << reg.AsRegister();
}

inline unsigned ExtractSRegister(uint32_t instr,
                                 int single_bit_field,
                                 int four_bit_field_lowest_bit) {
  VIXL_ASSERT(single_bit_field > 0);
  if (four_bit_field_lowest_bit == 0) {
    return ((instr << 1) & 0x1e) | ((instr >> single_bit_field) & 0x1);
  }
  return ((instr >> (four_bit_field_lowest_bit - 1)) & 0x1e) |
         ((instr >> single_bit_field) & 0x1);
}

inline std::ostream& operator<<(std::ostream& os, const SRegister reg) {
  return os << "s" << reg.GetCode();
}

inline unsigned ExtractDRegister(uint32_t instr,
                                 int single_bit_field,
                                 int four_bit_field_lowest_bit) {
  VIXL_ASSERT(single_bit_field >= 4);
  return ((instr >> (single_bit_field - 4)) & 0x10) |
         ((instr >> four_bit_field_lowest_bit) & 0xf);
}

inline std::ostream& operator<<(std::ostream& os, const DRegister reg) {
  return os << "d" << reg.GetCode();
}

inline std::ostream& operator<<(std::ostream& os, DataType dt) {
  return os << dt.GetName();
}

inline unsigned ExtractDRegisterAndLane(uint32_t instr,
                                        DataType dt,
                                        int single_bit_field,
                                        int four_bit_field_lowest_bit,
                                        int* lane) {
  VIXL_ASSERT(single_bit_field >= 4);
  uint32_t value = ((instr >> (single_bit_field - 4)) & 0x10) |
                   ((instr >> four_bit_field_lowest_bit) & 0xf);
  if (dt.GetSize() == 16) {
    *lane = value >> 3;
    return value & 0x7;
  }
  *lane = value >> 4;
  return value & 0xf;
}

inline std::ostream& operator<<(std::ostream& os, const DRegisterLane lane) {
  os << "d" << lane.GetCode() << "[";
  if (lane.GetLane() == static_cast<uint32_t>(-1)) return os << "??]";
  return os << lane.GetLane() << "]";
}

inline unsigned ExtractQRegister(uint32_t instr,
                                 int single_bit_field,
                                 int four_bit_field_lowest_bit) {
  VIXL_ASSERT(single_bit_field >= 3);
  return ((instr >> (single_bit_field - 3)) & 0x8) |
         ((instr >> (four_bit_field_lowest_bit + 1)) & 0x7);
}

inline std::ostream& operator<<(std::ostream& os, const QRegister reg) {
  return os << "q" << reg.GetCode();
}

// clang-format off
#define AARCH32_REGISTER_CODE_LIST(R)                                          \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)
// clang-format on
#define DEFINE_REGISTER(N) const Register r##N(N);
AARCH32_REGISTER_CODE_LIST(DEFINE_REGISTER)
#undef DEFINE_REGISTER
#undef AARCH32_REGISTER_CODE_LIST

enum RegNum { kIPRegNum = 12, kSPRegNum = 13, kLRRegNum = 14, kPCRegNum = 15 };

const Register ip(kIPRegNum);
const Register sp(kSPRegNum);
const Register pc(kPCRegNum);
const Register lr(kLRRegNum);
const VRegister NoVReg;

// clang-format off
#define SREGISTER_CODE_LIST(R)                                                 \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)                              \
  R(16) R(17) R(18) R(19) R(20) R(21) R(22) R(23)                              \
  R(24) R(25) R(26) R(27) R(28) R(29) R(30) R(31)
// clang-format on
#define DEFINE_REGISTER(N) const SRegister s##N(N);
SREGISTER_CODE_LIST(DEFINE_REGISTER)
#undef DEFINE_REGISTER
#undef SREGISTER_CODE_LIST
const SRegister NoSReg;

// clang-format off
#define DREGISTER_CODE_LIST(R)                                                 \
R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                                 \
R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)                                \
R(16) R(17) R(18) R(19) R(20) R(21) R(22) R(23)                                \
R(24) R(25) R(26) R(27) R(28) R(29) R(30) R(31)
// clang-format on
#define DEFINE_REGISTER(N) const DRegister d##N(N);
DREGISTER_CODE_LIST(DEFINE_REGISTER)
#undef DEFINE_REGISTER
#undef DREGISTER_CODE_LIST
const DRegister NoDReg;

// clang-format off
#define QREGISTER_CODE_LIST(R)                                                 \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)
// clang-format on
#define DEFINE_REGISTER(N) const QRegister q##N(N);
QREGISTER_CODE_LIST(DEFINE_REGISTER)
#undef DEFINE_REGISTER
#undef QREGISTER_CODE_LIST
const QRegister NoQReg;

inline uint32_t GetRegisterListEncoding(const RegisterList& registers,
                                        int first,
                                        int count) {
  return (registers.GetList() >> first) & ((1 << count) - 1);
}

std::ostream& operator<<(std::ostream& os, RegisterList registers);

class VRegisterList {
 public:
  VRegisterList() : list_(0) {}
  explicit VRegisterList(VRegister reg) : list_(RegisterToList(reg)) {}
  VRegisterList(VRegister reg1, VRegister reg2)
      : list_(RegisterToList(reg1) | RegisterToList(reg2)) {}
  VRegisterList(VRegister reg1, VRegister reg2, VRegister reg3)
      : list_(RegisterToList(reg1) | RegisterToList(reg2) |
              RegisterToList(reg3)) {}
  VRegisterList(VRegister reg1, VRegister reg2, VRegister reg3, VRegister reg4)
      : list_(RegisterToList(reg1) | RegisterToList(reg2) |
              RegisterToList(reg3) | RegisterToList(reg4)) {}
  explicit VRegisterList(uint64_t list) : list_(list) {}
  uint64_t GetList() const { return list_; }
  void SetList(uint64_t list) { list_ = list; }
  // Because differently-sized V registers overlap with one another, there is no
  // way to implement a single 'Includes' function in a way that is unsurprising
  // for all existing uses.
  bool IncludesAllOf(const VRegister& reg) const {
    return (list_ & RegisterToList(reg)) == RegisterToList(reg);
  }
  bool IncludesAliasOf(const VRegister& reg) const {
    return (list_ & RegisterToList(reg)) != 0;
  }
  void Combine(const VRegisterList& other) { list_ |= other.GetList(); }
  void Combine(const VRegister& reg) { list_ |= RegisterToList(reg); }
  void Remove(const VRegisterList& other) { list_ &= ~other.GetList(); }
  void Remove(const VRegister& reg) { list_ &= ~RegisterToList(reg); }
  bool Overlaps(const VRegisterList& other) const {
    return (list_ & other.list_) != 0;
  }
  QRegister GetFirstAvailableQRegister() const;
  DRegister GetFirstAvailableDRegister() const;
  SRegister GetFirstAvailableSRegister() const;
  bool IsEmpty() const { return list_ == 0; }
  static VRegisterList Union(const VRegisterList& list_1,
                             const VRegisterList& list_2) {
    return VRegisterList(list_1.list_ | list_2.list_);
  }
  static VRegisterList Union(const VRegisterList& list_1,
                             const VRegisterList& list_2,
                             const VRegisterList& list_3) {
    return Union(list_1, Union(list_2, list_3));
  }
  static VRegisterList Union(const VRegisterList& list_1,
                             const VRegisterList& list_2,
                             const VRegisterList& list_3,
                             const VRegisterList& list_4) {
    return Union(Union(list_1, list_2), Union(list_3, list_4));
  }
  static VRegisterList Intersection(const VRegisterList& list_1,
                                    const VRegisterList& list_2) {
    return VRegisterList(list_1.list_ & list_2.list_);
  }
  static VRegisterList Intersection(const VRegisterList& list_1,
                                    const VRegisterList& list_2,
                                    const VRegisterList& list_3) {
    return Intersection(list_1, Intersection(list_2, list_3));
  }
  static VRegisterList Intersection(const VRegisterList& list_1,
                                    const VRegisterList& list_2,
                                    const VRegisterList& list_3,
                                    const VRegisterList& list_4) {
    return Intersection(Intersection(list_1, list_2),
                        Intersection(list_3, list_4));
  }

 private:
  static uint64_t RegisterToList(VRegister reg) {
    if (reg.GetType() == CPURegister::kNoRegister) {
      return 0;
    } else {
      switch (reg.GetSizeInBits()) {
        case kQRegSizeInBits:
          return UINT64_C(0xf) << (reg.GetCode() * 4);
        case kDRegSizeInBits:
          return UINT64_C(0x3) << (reg.GetCode() * 2);
        case kSRegSizeInBits:
          return UINT64_C(0x1) << reg.GetCode();
        default:
          VIXL_UNREACHABLE();
          return 0;
      }
    }
  }

  // Bitfield representation of all registers in the list.
  // (0x3 for d0, 0xc0 for d1, 0x30 for d2, ...). We have one, two or four bits
  // per register according to their size. This way we can make sure that we
  // account for overlapping registers.
  // A register is wholly included in this list only if all of its bits are set.
  // A register is aliased by the list if at least one of its bits are set.
  // The IncludesAllOf and IncludesAliasOf helpers are provided to make this
  // distinction clear.
  uint64_t list_;
};

std::ostream& operator<<(std::ostream& os, SRegisterList registers);

std::ostream& operator<<(std::ostream& os, DRegisterList registers);

std::ostream& operator<<(std::ostream& os, NeonRegisterList registers);

inline std::ostream& operator<<(std::ostream& os, SpecialRegister reg) {
  return os << reg.GetName();
}

enum BankedRegisterType {
  R8_usr = 0x00,
  R9_usr = 0x01,
  R10_usr = 0x02,
  R11_usr = 0x03,
  R12_usr = 0x04,
  SP_usr = 0x05,
  LR_usr = 0x06,
  R8_fiq = 0x08,
  R9_fiq = 0x09,
  R10_fiq = 0x0a,
  R11_fiq = 0x0b,
  R12_fiq = 0x0c,
  SP_fiq = 0x0d,
  LR_fiq = 0x0e,
  LR_irq = 0x10,
  SP_irq = 0x11,
  LR_svc = 0x12,
  SP_svc = 0x13,
  LR_abt = 0x14,
  SP_abt = 0x15,
  LR_und = 0x16,
  SP_und = 0x17,
  LR_mon = 0x1c,
  SP_mon = 0x1d,
  ELR_hyp = 0x1e,
  SP_hyp = 0x1f,
  SPSR_fiq = 0x2e,
  SPSR_irq = 0x30,
  SPSR_svc = 0x32,
  SPSR_abt = 0x34,
  SPSR_und = 0x36,
  SPSR_mon = 0x3c,
  SPSR_hyp = 0x3e
};

class BankedRegister {
  uint32_t reg_;

 public:
  explicit BankedRegister(unsigned reg) : reg_(reg) {}
  BankedRegister(BankedRegisterType reg)  // NOLINT(runtime/explicit)
      : reg_(reg) {}
  uint32_t GetCode() const { return reg_; }
  const char* GetName() const;
};

inline std::ostream& operator<<(std::ostream& os, BankedRegister reg) {
  return os << reg.GetName();
}

inline std::ostream& operator<<(std::ostream& os, MaskedSpecialRegister reg) {
  return os << reg.GetName();
}

inline std::ostream& operator<<(std::ostream& os, SpecialFPRegister reg) {
  return os << reg.GetName();
}

class CRegister {
  uint32_t code_;

 public:
  explicit CRegister(uint32_t code) : code_(code) {
    VIXL_ASSERT(code < kNumberOfRegisters);
  }
  uint32_t GetCode() const { return code_; }
  bool Is(CRegister value) const { return code_ == value.code_; }
};

inline std::ostream& operator<<(std::ostream& os, const CRegister reg) {
  return os << "c" << reg.GetCode();
}

// clang-format off
#define CREGISTER_CODE_LIST(R)                                                 \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)
// clang-format on
#define DEFINE_CREGISTER(N) const CRegister c##N(N);
CREGISTER_CODE_LIST(DEFINE_CREGISTER)

enum CoprocessorName { p10 = 10, p11 = 11, p14 = 14, p15 = 15 };

class Coprocessor {
  uint32_t coproc_;

 public:
  explicit Coprocessor(uint32_t coproc) : coproc_(coproc) {}
  Coprocessor(CoprocessorName coproc)  // NOLINT(runtime/explicit)
      : coproc_(static_cast<uint32_t>(coproc)) {}
  bool Is(Coprocessor coproc) const { return coproc_ == coproc.coproc_; }
  bool Is(CoprocessorName coproc) const { return coproc_ == coproc; }
  uint32_t GetCoprocessor() const { return coproc_; }
};

inline std::ostream& operator<<(std::ostream& os, Coprocessor coproc) {
  return os << "p" << coproc.GetCoprocessor();
}

inline std::ostream& operator<<(std::ostream& os, Condition condition) {
  return os << condition.GetName();
}

inline std::ostream& operator<<(std::ostream& os, Sign sign) {
  return os << sign.GetName();
}

inline std::ostream& operator<<(std::ostream& os, Shift shift) {
  return os << shift.GetName();
}

class ImmediateShiftOperand : public Shift {
 public:
  // Constructor used for assembly.
  ImmediateShiftOperand(Shift shift, uint32_t amount)
      : Shift(shift), amount_(amount) {
#ifdef VIXL_DEBUG
    switch (shift.GetType()) {
      case LSL:
        VIXL_ASSERT(amount <= 31);
        break;
      case ROR:
        VIXL_ASSERT(amount > 0);
        VIXL_ASSERT(amount <= 31);
        break;
      case LSR:
      case ASR:
        VIXL_ASSERT(amount > 0);
        VIXL_ASSERT(amount <= 32);
        break;
      case RRX:
        VIXL_ASSERT(amount == 0);
        break;
      default:
        VIXL_UNREACHABLE();
        break;
    }
#endif
  }
  // Constructor used for disassembly.
  ImmediateShiftOperand(int shift, int amount);
  uint32_t GetAmount() const { return amount_; }
  bool Is(const ImmediateShiftOperand& rhs) const {
    return amount_ == (rhs.amount_) && Shift::Is(*this);
  }

 private:
  uint32_t amount_;
};

inline std::ostream& operator<<(std::ostream& os,
                                ImmediateShiftOperand const& shift_operand) {
  if (shift_operand.IsLSL() && shift_operand.GetAmount() == 0) return os;
  if (shift_operand.IsRRX()) return os << ", rrx";
  return os << ", " << shift_operand.GetName() << " #"
            << shift_operand.GetAmount();
}

class RegisterShiftOperand : public Shift {
 public:
  RegisterShiftOperand(ShiftType shift, Register shift_register)
      : Shift(shift), shift_register_(shift_register) {
    VIXL_ASSERT(!IsRRX() && shift_register_.IsValid());
  }
  const Register GetShiftRegister() const { return shift_register_; }
  bool Is(const RegisterShiftOperand& rhs) const {
    return shift_register_.Is(rhs.shift_register_) && Shift::Is(*this);
  }

 private:
  Register shift_register_;
};

inline std::ostream& operator<<(std::ostream& s,
                                const RegisterShiftOperand& shift_operand) {
  return s << shift_operand.GetName() << " "
           << shift_operand.GetShiftRegister();
}

enum EncodingSizeType { Best, Narrow, Wide };

class EncodingSize {
  uint32_t size_;

 public:
  explicit EncodingSize(uint32_t size) : size_(size) {}
  EncodingSize(EncodingSizeType size)  // NOLINT(runtime/explicit)
      : size_(size) {}
  uint32_t GetSize() const { return size_; }
  const char* GetName() const;
  bool IsBest() const { return size_ == Best; }
  bool IsNarrow() const { return size_ == Narrow; }
  bool IsWide() const { return size_ == Wide; }
};

inline std::ostream& operator<<(std::ostream& os, EncodingSize size) {
  return os << size.GetName();
}

inline std::ostream& operator<<(std::ostream& os, WriteBack write_back) {
  if (write_back.DoesWriteBack()) return os << "!";
  return os;
}

class EncodingValue {
  bool valid_;
  uint32_t encoding_value_;

 public:
  EncodingValue() {
    valid_ = false;
    encoding_value_ = 0;
  }
  bool IsValid() const { return valid_; }
  uint32_t GetEncodingValue() const { return encoding_value_; }
  void SetEncodingValue(uint32_t encoding_value) {
    valid_ = true;
    encoding_value_ = encoding_value;
  }
};

class EncodingValueAndImmediate : public EncodingValue {
  uint32_t encoded_immediate_;

 public:
  EncodingValueAndImmediate() { encoded_immediate_ = 0; }
  uint32_t GetEncodedImmediate() const { return encoded_immediate_; }
  void SetEncodedImmediate(uint32_t encoded_immediate) {
    encoded_immediate_ = encoded_immediate;
  }
};

class ImmediateT32 : public EncodingValue {
 public:
  explicit ImmediateT32(uint32_t imm);
  static bool IsImmediateT32(uint32_t imm);
  static uint32_t Decode(uint32_t value);
};

class ImmediateA32 : public EncodingValue {
 public:
  explicit ImmediateA32(uint32_t imm);
  static bool IsImmediateA32(uint32_t imm);
  static uint32_t Decode(uint32_t value);
};

// Return the encoding value of a shift type.
uint32_t TypeEncodingValue(Shift shift);
// Return the encoding value for a shift amount depending on the shift type.
uint32_t AmountEncodingValue(Shift shift, uint32_t amount);

inline std::ostream& operator<<(std::ostream& os, MemoryBarrier option) {
  return os << option.GetName();
}

enum InterruptFlagsType {
  F = 0x1,
  I = 0x2,
  IF = 0x3,
  A = 0x4,
  AF = 0x5,
  AI = 0x6,
  AIF = 0x7
};

class InterruptFlags {
  InterruptFlagsType type_;

 public:
  InterruptFlags(InterruptFlagsType type)  // NOLINT(runtime/explicit)
      : type_(type) {}
  InterruptFlags(uint32_t type)  // NOLINT(runtime/explicit)
      : type_(static_cast<InterruptFlagsType>(type)) {
    VIXL_ASSERT(type <= 7);
  }
  InterruptFlagsType GetType() const { return type_; }
  const char* GetName() const;
};

inline std::ostream& operator<<(std::ostream& os, InterruptFlags option) {
  return os << option.GetName();
}

enum EndiannessType { LE = 0, BE = 1 };

class Endianness {
  EndiannessType type_;

 public:
  Endianness(EndiannessType type) : type_(type) {}  // NOLINT(runtime/explicit)
  Endianness(uint32_t type)                         // NOLINT(runtime/explicit)
      : type_(static_cast<EndiannessType>(type)) {
    VIXL_ASSERT(type <= 1);
  }
  EndiannessType GetType() const { return type_; }
  const char* GetName() const;
};

inline std::ostream& operator<<(std::ostream& os, Endianness endian_specifier) {
  return os << endian_specifier.GetName();
}

enum AlignmentType {
  k16BitAlign = 0,
  k32BitAlign = 1,
  k64BitAlign = 2,
  k128BitAlign = 3,
  k256BitAlign = 4,
  kNoAlignment = 5,
  kBadAlignment = 6
};

class Alignment {
  AlignmentType align_;

 public:
  Alignment(AlignmentType align)  // NOLINT(runtime/explicit)
      : align_(align) {}
  Alignment(uint32_t align)  // NOLINT(runtime/explicit)
      : align_(static_cast<AlignmentType>(align)) {
    VIXL_ASSERT(align <= static_cast<uint32_t>(k256BitAlign));
  }
  AlignmentType GetType() const { return align_; }
  bool Is(AlignmentType type) { return align_ == type; }
};

inline std::ostream& operator<<(std::ostream& os, Alignment align) {
  if (align.GetType() == kBadAlignment) return os << " :??";
  if (align.GetType() == kNoAlignment) return os;
  return os << " :" << (0x10 << static_cast<uint32_t>(align.GetType()));
}

// Structure containing information on forward references.
struct ReferenceInfo {
  int size;
  int min_offset;
  int max_offset;
  int alignment;  // As a power of two.
  enum { kAlignPc, kDontAlignPc } pc_needs_aligning;
};

}  // namespace aarch32
}  // namespace vixl

#endif  // VIXL_AARCH32_INSTRUCTIONS_AARCH32_H_
