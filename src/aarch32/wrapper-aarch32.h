#ifndef VIXL_AARCH32_WRAPPER_AARCH32_H_
#define VIXL_AARCH32_WRAPPER_AARCH32_H_

#include <stdint.h>

const unsigned kRegSizeInBits = 32;
const unsigned kRegSizeInBytes = kRegSizeInBits / 8;
const unsigned kSRegSizeInBits = 32;
const unsigned kSRegSizeInBytes = kSRegSizeInBits / 8;
const unsigned kDRegSizeInBits = 64;
const unsigned kDRegSizeInBytes = kDRegSizeInBits / 8;
const unsigned kQRegSizeInBits = 128;
const unsigned kQRegSizeInBytes = kQRegSizeInBits / 8;

const unsigned kNumberOfRegisters = 16;
const unsigned kNumberOfSRegisters = 32;
const unsigned kMaxNumberOfDRegisters = 32;
const unsigned kNumberOfQRegisters = 16;
const unsigned kNumberOfT32LowRegisters = 8;

const unsigned kIpCode = 12;
const unsigned kSpCode = 13;
const unsigned kLrCode = 14;
const unsigned kPcCode = 15;

const unsigned kT32PcDelta = 4;
const unsigned kA32PcDelta = 8;

const unsigned kRRXEncodedValue = 3;

const unsigned kCoprocMask = 0xe;
const unsigned kInvalidCoprocMask = 0xa;

const unsigned kLowestT32_32Opcode = 0xe8000000;

const uint32_t kUnknownValue = 0xdeadbeef;

const uint32_t kMaxInstructionSizeInBytes = 4;
const uint32_t kA32InstructionSizeInBytes = 4;
const uint32_t k32BitT32InstructionSizeInBytes = 4;
const uint32_t k16BitT32InstructionSizeInBytes = 2;

// Maximum size emitted by a single T32 unconditional macro-instruction.
const uint32_t kMaxT32MacroInstructionSizeInBytes = 32;

const uint32_t kCallerSavedRegistersMask = 0x500f;

const uint16_t k16BitT32NopOpcode = 0xbf00;
const uint16_t kCbzCbnzMask = 0xf500;
const uint16_t kCbzCbnzValue = 0xb100;

const int32_t kCbzCbnzRange = 126;
const int32_t kBConditionalNarrowRange = 254;
const int32_t kBNarrowRange = 2046;
const int32_t kNearLabelRange = kBNarrowRange;

enum FlagsUpdate { LeaveFlags = 0, SetFlags = 1, DontCare = 2 };

enum BranchHint { kNear, kFar, kBranchWithoutHint };

enum ConditionType {
  eq = 0,
  ne = 1,
  cs = 2,
  cc = 3,
  mi = 4,
  pl = 5,
  vs = 6,
  vc = 7,
  hi = 8,
  ls = 9,
  ge = 10,
  lt = 11,
  gt = 12,
  le = 13,
  al = 14,
  hs = cs,
  lo = cc
};

class Condition {
  uint32_t condition_;
  static const uint32_t kNever = 15;
  static const uint32_t kMask = 0xf;
  static const uint32_t kNone = 0x10 | al;

 public:
  static const Condition None() { return Condition(kNone); }
  static const Condition Never() { return Condition(kNever); }
  explicit Condition(uint32_t condition);
  // Users should be able to use "eq", "ne" and so forth to instantiate this
  // class.
  Condition(ConditionType condition)  // NOLINT(runtime/explicit)
      : condition_(condition) {}
  uint32_t GetCondition() const { return condition_ & kMask; }
  bool IsNone() const { return condition_ == kNone; }
  const char* GetName() const;
  bool Is(Condition value) const { return condition_ == value.condition_; }
  bool Is(uint32_t value) const { return condition_ == value; }
  bool IsNot(uint32_t value) const { return condition_ != value; }
  bool IsNever() const { return condition_ == kNever; }
  bool IsNotNever() const { return condition_ != kNever; }
  Condition Negate() const;
};

enum DataTypeType {
  kDataTypeS = 0x100,
  kDataTypeU = 0x200,
  kDataTypeF = 0x300,
  kDataTypeI = 0x400,
  kDataTypeP = 0x500,
  kDataTypeUntyped = 0x600
};
const int kDataTypeSizeMask = 0x0ff;
const int kDataTypeTypeMask = 0x100;
enum DataTypeValue {
  kDataTypeValueInvalid = 0x000,
  kDataTypeValueNone = 0x001,  // value used when dt is ignored.
  S8 = kDataTypeS | 8,
  S16 = kDataTypeS | 16,
  S32 = kDataTypeS | 32,
  S64 = kDataTypeS | 64,
  U8 = kDataTypeU | 8,
  U16 = kDataTypeU | 16,
  U32 = kDataTypeU | 32,
  U64 = kDataTypeU | 64,
  F16 = kDataTypeF | 16,
  F32 = kDataTypeF | 32,
  F64 = kDataTypeF | 64,
  I8 = kDataTypeI | 8,
  I16 = kDataTypeI | 16,
  I32 = kDataTypeI | 32,
  I64 = kDataTypeI | 64,
  P8 = kDataTypeP | 8,
  P64 = kDataTypeP | 64,
  Untyped8 = kDataTypeUntyped | 8,
  Untyped16 = kDataTypeUntyped | 16,
  Untyped32 = kDataTypeUntyped | 32,
  Untyped64 = kDataTypeUntyped | 64
};

class DataType {
  DataTypeValue value_;

 public:
  explicit DataType(uint32_t size);
  // Users should be able to use "S8", "S6" and so forth to instantiate this
  // class.
  DataType(DataTypeValue value) : value_(value) {}  // NOLINT(runtime/explicit)
  DataTypeValue GetValue() const { return value_; }
  DataTypeType GetType() const {
    return static_cast<DataTypeType>(value_ & kDataTypeTypeMask);
  }
  uint32_t GetSize() const { return value_ & kDataTypeSizeMask; }
  bool IsSize(uint32_t size) const {
    return (value_ & kDataTypeSizeMask) == size;
  }
  const char* GetName() const;
  bool Is(DataType type) const { return value_ == type.value_; }
  bool Is(DataTypeValue value) const { return value_ == value; }
  bool Is(DataTypeType type) const { return GetType() == type; }
  bool IsNoneOr(DataTypeValue value) const {
    return (value_ == value) || (value_ == kDataTypeValueNone);
  }
  bool Is(DataTypeType type, uint32_t size) const {
    return value_ == static_cast<DataTypeValue>(type | size);
  }
  bool IsNoneOr(DataTypeType type, uint32_t size) const {
    return Is(type, size) || Is(kDataTypeValueNone);
  }
};

enum ShiftType { LSL = 0x0, LSR = 0x1, ASR = 0x2, ROR = 0x3, RRX = 0x4 };

class Shift {
 public:
  Shift() : shift_(LSL) {}
  Shift(ShiftType shift) : shift_(shift) {}  // NOLINT(runtime/explicit)
  explicit Shift(uint32_t shift) : shift_(static_cast<ShiftType>(shift)) {}
  const Shift& GetShift() const { return *this; }
  ShiftType GetType() const { return shift_; }
  uint32_t GetValue() const { return shift_; }
  const char* GetName() const;
  bool IsLSL() const { return shift_ == LSL; }
  bool IsLSR() const { return shift_ == LSR; }
  bool IsASR() const { return shift_ == ASR; }
  bool IsROR() const { return shift_ == ROR; }
  bool IsRRX() const { return shift_ == RRX; }
  bool Is(Shift value) const { return shift_ == value.shift_; }
  bool IsNot(Shift value) const { return shift_ != value.shift_; }
  bool IsValidAmount(uint32_t amount) const;
  static const Shift NoShift;

 protected:
  void SetType(ShiftType s) { shift_ = s; }

 private:
  ShiftType shift_;
};

class CPURegister {
 public:
  enum RegisterType {
    kNoRegister = 0,
    kRRegister = 1,
    kSRegister = 2,
    kDRegister = 3,
    kQRegister = 4
  };

 private:
  static const int kCodeBits = 5;
  static const int kTypeBits = 4;
  static const int kSizeBits = 8;
  static const int kCodeShift = 0;
  static const int kTypeShift = kCodeShift + kCodeBits;
  static const int kSizeShift = kTypeShift + kTypeBits;
  static const uint32_t kCodeMask = ((1 << kCodeBits) - 1) << kCodeShift;
  static const uint32_t kTypeMask = ((1 << kTypeBits) - 1) << kTypeShift;
  static const uint32_t kSizeMask = ((1 << kSizeBits) - 1) << kSizeShift;
  uint32_t value_;

 public:
  CPURegister(RegisterType type, uint32_t code, int size);
  RegisterType GetType() const {
    return static_cast<RegisterType>((value_ & kTypeMask) >> kTypeShift);
  }
  bool IsRegister() const { return GetType() == kRRegister; }
  bool IsS() const { return GetType() == kSRegister; }
  bool IsD() const { return GetType() == kDRegister; }
  bool IsQ() const { return GetType() == kQRegister; }
  bool IsVRegister() const { return IsS() || IsD() || IsQ(); }
  bool IsFPRegister() const { return IsS() || IsD(); }
  uint32_t GetCode() const { return (value_ & kCodeMask) >> kCodeShift; }
  uint32_t GetReg() const { return value_; }
  int GetSizeInBits() const { return (value_ & kSizeMask) >> kSizeShift; }
  int GetRegSizeInBytes() const {
    return (GetType() == kNoRegister) ? 0 : (GetSizeInBits() / 8);
  }
  bool Is64Bits() const { return GetSizeInBits() == 64; }
  bool Is128Bits() const { return GetSizeInBits() == 128; }
  bool IsSameFormat(CPURegister reg) {
    return (value_ & ~kCodeMask) == (reg.value_ & ~kCodeMask);
  }
  bool Is(CPURegister ref) const { return GetReg() == ref.GetReg(); }
  bool IsValid() const { return GetType() != kNoRegister; }
};

class Register : public CPURegister {
 public:
  Register() : CPURegister(kNoRegister, 0, kRegSizeInBits) {}
  explicit Register(uint32_t code);
  bool Is(Register ref) const { return GetCode() == ref.GetCode(); }
  bool IsLow() const { return GetCode() < kNumberOfT32LowRegisters; }
  bool IsLR() const { return GetCode() == kLrCode; }
  bool IsPC() const { return GetCode() == kPcCode; }
  bool IsSP() const { return GetCode() == kSpCode; }
};

class SRegister;
class DRegister;
class QRegister;

class VRegister : public CPURegister {
 public:
  VRegister() : CPURegister(kNoRegister, 0, 0) {}
  VRegister(RegisterType type, uint32_t code, int size)
      : CPURegister(type, code, size) {}

  SRegister S() const;
  DRegister D() const;
  QRegister Q() const;
};

class SRegister : public VRegister {
 public:
  SRegister() : VRegister(kNoRegister, 0, kSRegSizeInBits) {}
  explicit SRegister(uint32_t code)
      : VRegister(kSRegister, code, kSRegSizeInBits) {}
  uint32_t Encode(int single_bit_field, int four_bit_field_lowest_bit) const {
    if (four_bit_field_lowest_bit == 0) {
      return ((GetCode() & 0x1) << single_bit_field) |
             ((GetCode() & 0x1e) >> 1);
    }
    return ((GetCode() & 0x1) << single_bit_field) |
           ((GetCode() & 0x1e) << (four_bit_field_lowest_bit - 1));
  }
};

class DRegister : public VRegister {
 public:
  DRegister() : VRegister(kNoRegister, 0, kDRegSizeInBits) {}
  explicit DRegister(uint32_t code)
      : VRegister(kDRegister, code, kDRegSizeInBits) {}
  SRegister GetLane(uint32_t lane) const;
  uint32_t Encode(int single_bit_field, int four_bit_field_lowest_bit) const;
};

class QRegister : public VRegister {
 public:
  QRegister() : VRegister(kNoRegister, 0, kQRegSizeInBits) {}
  explicit QRegister(uint32_t code)
      : VRegister(kQRegister, code, kQRegSizeInBits) {}
  uint32_t Encode(int offset) { return GetCode() << offset; }
  DRegister GetDLane(uint32_t lane) const;
  DRegister GetLowDRegister() const { return DRegister(GetCode() * 2); }
  DRegister GetHighDRegister() const { return DRegister(1 + GetCode() * 2); }
  SRegister GetSLane(uint32_t lane) const;
  uint32_t Encode(int single_bit_field, int four_bit_field_lowest_bit);
};

class DRegisterLane : public DRegister {
  uint32_t lane_;

 public:
  DRegisterLane(DRegister reg, uint32_t lane)
      : DRegister(reg.GetCode()), lane_(lane) {}
  DRegisterLane(uint32_t code, uint32_t lane) : DRegister(code), lane_(lane) {}
  uint32_t GetLane() const { return lane_; }
  uint32_t EncodeX(DataType dt,
                   int single_bit_field,
                   int four_bit_field_lowest_bit) const;
};

class RegisterList {
 public:
  RegisterList() : list_(0) {}
  RegisterList(Register reg)  // NOLINT(runtime/explicit)
      : list_(RegisterToList(reg)) {}
  RegisterList(Register reg1, Register reg2)
      : list_(RegisterToList(reg1) | RegisterToList(reg2)) {}
  RegisterList(Register reg1, Register reg2, Register reg3)
      : list_(RegisterToList(reg1) | RegisterToList(reg2) |
              RegisterToList(reg3)) {}
  RegisterList(Register reg1, Register reg2, Register reg3, Register reg4)
      : list_(RegisterToList(reg1) | RegisterToList(reg2) |
              RegisterToList(reg3) | RegisterToList(reg4)) {}
  explicit RegisterList(uint32_t list) : list_(list) {}
  uint32_t GetList() const { return list_; }
  void SetList(uint32_t list) { list_ = list; }
  bool Includes(const Register& reg) const {
    return (list_ & RegisterToList(reg)) != 0;
  }
  void Combine(const RegisterList& other) { list_ |= other.GetList(); }
  void Combine(const Register& reg) { list_ |= RegisterToList(reg); }
  void Remove(const RegisterList& other) { list_ &= ~other.GetList(); }
  void Remove(const Register& reg) { list_ &= ~RegisterToList(reg); }
  bool Overlaps(const RegisterList& other) const {
    return (list_ & other.list_) != 0;
  }
  bool IsR0toR7orPC() const {
    // True if all the registers from the list are not from r8-r14.
    return (list_ & 0x7f00) == 0;
  }
  bool IsR0toR7orLR() const {
    // True if all the registers from the list are not from r8-r13 nor from r15.
    return (list_ & 0xbf00) == 0;
  }
  Register GetFirstAvailableRegister() const;
  bool IsEmpty() const { return list_ == 0; }
  bool IsSingleRegister() const;
  int GetCount() const;
  static RegisterList Union(const RegisterList& list_1,
                            const RegisterList& list_2) {
    return RegisterList(list_1.list_ | list_2.list_);
  }
  static RegisterList Union(const RegisterList& list_1,
                            const RegisterList& list_2,
                            const RegisterList& list_3) {
    return Union(list_1, Union(list_2, list_3));
  }
  static RegisterList Union(const RegisterList& list_1,
                            const RegisterList& list_2,
                            const RegisterList& list_3,
                            const RegisterList& list_4) {
    return Union(Union(list_1, list_2), Union(list_3, list_4));
  }
  static RegisterList Intersection(const RegisterList& list_1,
                                   const RegisterList& list_2) {
    return RegisterList(list_1.list_ & list_2.list_);
  }
  static RegisterList Intersection(const RegisterList& list_1,
                                   const RegisterList& list_2,
                                   const RegisterList& list_3) {
    return Intersection(list_1, Intersection(list_2, list_3));
  }
  static RegisterList Intersection(const RegisterList& list_1,
                                   const RegisterList& list_2,
                                   const RegisterList& list_3,
                                   const RegisterList& list_4) {
    return Intersection(Intersection(list_1, list_2),
                        Intersection(list_3, list_4));
  }

 private:
  static uint32_t RegisterToList(Register reg) {
    if (reg.GetType() == CPURegister::kNoRegister) {
      return 0;
    } else {
      return UINT32_C(1) << reg.GetCode();
    }
  }

  // Bitfield representation of all registers in the list
  // (1 for r0, 2 for r1, 4 for r2, ...).
  uint32_t list_;
};

enum SpacingType { kSingle, kDouble };

enum TransferType { kMultipleLanes, kOneLane, kAllLanes };

class NeonRegisterList {
  DRegister first_;
  SpacingType spacing_;
  TransferType type_;
  int lane_;
  int length_;

 public:
  NeonRegisterList(DRegister reg, TransferType type);
  NeonRegisterList(DRegister reg, int lane);
  NeonRegisterList(DRegister first,
                   DRegister last,
                   SpacingType spacing,
                   TransferType type);
  NeonRegisterList(DRegister first,
                   DRegister last,
                   SpacingType spacing,
                   int lane);
  DRegister GetDRegister(int n) const;
  const DRegister& GetFirstDRegister() const { return first_; }
  DRegister GetLastDRegister() const { return GetDRegister(length_ - 1); }
  int GetLength() const { return length_; }
  bool IsSingleSpaced() const { return spacing_ == kSingle; }
  bool IsDoubleSpaced() const { return spacing_ == kDouble; }
  bool IsTransferAllLanes() const { return type_ == kAllLanes; }
  bool IsTransferOneLane() const { return type_ == kOneLane; }
  bool IsTransferMultipleLanes() const { return type_ == kMultipleLanes; }
  int GetTransferLane() const { return lane_; }
};

class DRegisterList {
  DRegister first_;
  int length_;

 public:
  explicit DRegisterList(DRegister reg) : first_(reg.GetCode()), length_(1) {}
  DRegisterList(DRegister first, int length);
  DRegister GetDRegister(int n) const;
  const DRegister& GetFirstDRegister() const { return first_; }
  DRegister GetLastDRegister() const { return GetDRegister(length_ - 1); }
  int GetLength() const { return length_; }
};

enum SpecialRegisterType { APSR = 0, CPSR = 0, SPSR = 1 };

class SpecialRegister {
  uint32_t reg_;

 public:
  explicit SpecialRegister(uint32_t reg) : reg_(reg) {}
  SpecialRegister(SpecialRegisterType reg)  // NOLINT(runtime/explicit)
      : reg_(reg) {}
  uint32_t GetReg() const { return reg_; }
  const char* GetName() const;
  bool Is(SpecialRegister value) const { return reg_ == value.reg_; }
  bool Is(uint32_t value) const { return reg_ == value; }
  bool IsNot(uint32_t value) const { return reg_ != value; }
};

enum MaskedSpecialRegisterType {
  APSR_nzcvq = 0x08,
  APSR_g = 0x04,
  APSR_nzcvqg = 0x0c,
  CPSR_c = 0x01,
  CPSR_x = 0x02,
  CPSR_xc = 0x03,
  CPSR_s = APSR_g,
  CPSR_sc = 0x05,
  CPSR_sx = 0x06,
  CPSR_sxc = 0x07,
  CPSR_f = APSR_nzcvq,
  CPSR_fc = 0x09,
  CPSR_fx = 0x0a,
  CPSR_fxc = 0x0b,
  CPSR_fs = APSR_nzcvqg,
  CPSR_fsc = 0x0d,
  CPSR_fsx = 0x0e,
  CPSR_fsxc = 0x0f,
  SPSR_c = 0x11,
  SPSR_x = 0x12,
  SPSR_xc = 0x13,
  SPSR_s = 0x14,
  SPSR_sc = 0x15,
  SPSR_sx = 0x16,
  SPSR_sxc = 0x17,
  SPSR_f = 0x18,
  SPSR_fc = 0x19,
  SPSR_fx = 0x1a,
  SPSR_fxc = 0x1b,
  SPSR_fs = 0x1c,
  SPSR_fsc = 0x1d,
  SPSR_fsx = 0x1e,
  SPSR_fsxc = 0x1f
};

class MaskedSpecialRegister {
  uint32_t reg_;

 public:
  explicit MaskedSpecialRegister(uint32_t reg);
  MaskedSpecialRegister(MaskedSpecialRegisterType reg) : reg_(reg) {}
  uint32_t GetReg() const { return reg_; }
  const char* GetName() const;
  bool Is(MaskedSpecialRegister value) const { return reg_ == value.reg_; }
  bool Is(uint32_t value) const { return reg_ == value; }
  bool IsNot(uint32_t value) const { return reg_ != value; }
};

class SRegisterList {
  SRegister first_;
  int length_;

 public:
  explicit SRegisterList(SRegister reg) : first_(reg.GetCode()), length_(1) {}
  SRegisterList(SRegister first, int length);
  SRegister GetSRegister(int n) const;
  const SRegister& GetFirstSRegister() const { return first_; }
  SRegister GetLastSRegister() const { return GetSRegister(length_ - 1); }
  int GetLength() const { return length_; }
};

enum SpecialFPRegisterType {
  FPSID = 0x0,
  FPSCR = 0x1,
  MVFR2 = 0x5,
  MVFR1 = 0x6,
  MVFR0 = 0x7,
  FPEXC = 0x8
};

class SpecialFPRegister {
  uint32_t reg_;

 public:
  explicit SpecialFPRegister(uint32_t reg);
  SpecialFPRegister(SpecialFPRegisterType reg) : reg_(reg) {}
  uint32_t GetReg() const { return reg_; }
  const char* GetName() const;
  bool Is(SpecialFPRegister value) const { return reg_ == value.reg_; }
  bool Is(uint32_t value) const { return reg_ == value; }
  bool IsNot(uint32_t value) const { return reg_ != value; }
};

enum WriteBackValue { NO_WRITE_BACK, WRITE_BACK };

class WriteBack {
  WriteBackValue value_;

 public:
  WriteBack(WriteBackValue value) : value_(value) {}
  explicit WriteBack(int value)
      : value_((value == 0) ? NO_WRITE_BACK : WRITE_BACK) {}
  uint32_t GetWriteBackUint32() const { return (value_ == WRITE_BACK) ? 1 : 0; }
  bool DoesWriteBack() const { return value_ == WRITE_BACK; }
};

enum MemoryBarrierType {
  OSHLD = 0x1,
  OSHST = 0x2,
  OSH = 0x3,
  NSHLD = 0x5,
  NSHST = 0x6,
  NSH = 0x7,
  ISHLD = 0x9,
  ISHST = 0xa,
  ISH = 0xb,
  LD = 0xd,
  ST = 0xe,
  SY = 0xf
};

class MemoryBarrier {
  MemoryBarrierType type_;

 public:
  MemoryBarrier(MemoryBarrierType type) : type_(type) {}
  MemoryBarrier(uint32_t type);
  MemoryBarrierType GetType() const { return type_; }
  const char* GetName() const;
};

enum PlacementPolicy { kPlacedWhenUsed, kManuallyPlaced };

enum DeletionPolicy {
  kDeletedOnPlacementByPool,
  kDeletedOnPoolDestruction,
  kManuallyDeleted
};

class RegisterOrAPSR_nzcv {
  uint32_t code_;

 public:
  explicit RegisterOrAPSR_nzcv(uint32_t code);
  bool IsAPSR_nzcv() const { return code_ == kPcCode; }
  uint32_t GetCode() const { return code_; }
  Register AsRegister() const;
};

enum AddrMode { Offset = 0, PreIndex = 1, PostIndex = 2 };

enum SignType { plus, minus };

class Sign {
 public:
  Sign() : sign_(plus) {}
  Sign(SignType sign) : sign_(sign) {}
  const char* GetName() const { return (IsPlus() ? "" : "-"); }
  bool IsPlus() const { return sign_ == plus; }
  bool IsMinus() const { return sign_ == minus; }
  int32_t ApplyTo(uint32_t value);

 private:
  SignType sign_;
};

const Register NoReg;

// MemOperand represents the addressing mode of a load or store instruction.
//
//   Usage: <instr> <Rt> , <MemOperand>
//
//   where <instr> is the instruction to use (e.g., Ldr(), Str(), etc.),
//         <Rt> is general purpose register to be transferred,
//         <MemOperand> is the rest of the arguments to the instruction
//
//   <MemOperand> can be in one of 3 addressing modes:
//
//   [ <Rn>, <offset> ]   ==  offset addressing
//   [ <Rn>, <offset> ]!  ==  pre-indexed addressing
//   [ <Rn> ], <offset>   ==  post-indexed addressing
//
//   where <offset> can be one of:
//     - an immediate constant, such as <imm8>, <imm12>
//     - an index register <Rm>
//     - a shifted index register <Rm>, <shift> #<amount>
//
//   The index register may have an associated {+/-} sign,
//   which if omitted, defaults to + .
//
//   We have two constructors for the offset:
//
//   One with a signed value offset parameter. The value of sign_ is
//   "sign_of(constructor's offset parameter) and the value of offset_ is
//   "constructor's offset parameter".
//
//   The other with a sign and a positive value offset parameters. The value of
//   sign_ is "constructor's sign parameter" and the value of offset_ is
//   "constructor's sign parameter * constructor's offset parameter".
//
//   The value of offset_ reflects the effective offset. For an offset_ of 0,
//   sign_ can be positive or negative. Otherwise, sign_ always agrees with
//   the sign of offset_.
class MemOperand {
 public:
  // rn
  // where rn is the general purpose base register only
  explicit MemOperand(Register rn, AddrMode addrmode = Offset);

  // rn, #<imm>
  // where rn is the general purpose base register,
  //       <imm> is a 32-bit offset to add to rn
  //
  // Note: if rn is PC, then this form is equivalent to a "label"
  // Note: the second constructor allow minus zero (-0).
  MemOperand(Register rn, int32_t offset, AddrMode addrmode = Offset);
  MemOperand(Register rn,
             Sign sign,
             int32_t offset,
             AddrMode addrmode = Offset);

  // rn, {+/-}rm
  // where rn is the general purpose base register,
  //       {+/-} is the sign of the index register,
  //       rm is the general purpose index register,
  MemOperand(Register rn, Sign sign, Register rm, AddrMode addrmode = Offset);

  // rn, rm
  // where rn is the general purpose base register,
  //       rm is the general purpose index register,
  MemOperand(Register rn, Register rm, AddrMode addrmode = Offset);

  // rn, {+/-}rm, <shift>
  // where rn is the general purpose base register,
  //       {+/-} is the sign of the index register,
  //       rm is the general purpose index register,
  //       <shift> is RRX, applied to value from rm
  MemOperand(Register rn,
             Sign sign,
             Register rm,
             Shift shift,
             AddrMode addrmode = Offset);

  // rn, rm, <shift>
  // where rn is the general purpose base register,
  //       rm is the general purpose index register,
  //       <shift> is RRX, applied to value from rm
  MemOperand(Register rn, Register rm, Shift shift, AddrMode addrmode = Offset);

  // rn, {+/-}rm, <shift> #<amount>
  // where rn is the general purpose base register,
  //       {+/-} is the sign of the index register,
  //       rm is the general purpose index register,
  //       <shift> is one of {LSL, LSR, ASR, ROR}, applied to value from rm
  //       <shift_amount> is optional size to apply to value from rm
  MemOperand(Register rn,
             Sign sign,
             Register rm,
             Shift shift,
             uint32_t shift_amount,
             AddrMode addrmode = Offset);

  // rn, rm, <shift> #<amount>
  // where rn is the general purpose base register,
  //       rm is the general purpose index register,
  //       <shift> is one of {LSL, LSR, ASR, ROR}, applied to value from rm
  //       <shift_amount> is optional size to apply to value from rm
  MemOperand(Register rn,
             Register rm,
             Shift shift,
             uint32_t shift_amount,
             AddrMode addrmode = Offset);

  Register GetBaseRegister() const { return rn_; }
  int32_t GetOffsetImmediate() const { return offset_; }
  bool IsOffsetImmediateWithinRange(int min,
                                    int max,
                                    int multiple_of = 1) const {
    return (offset_ >= min) && (offset_ <= max) &&
           ((offset_ % multiple_of) == 0);
  }
  Sign GetSign() const { return sign_; }
  Register GetOffsetRegister() const { return rm_; }
  Shift GetShift() const { return shift_; }
  unsigned GetShiftAmount() const { return shift_amount_; }
  AddrMode GetAddrMode() const {
    return static_cast<AddrMode>(addrmode_ & kMemOperandAddrModeMask);
  }
  bool IsRegisterOnly() const {
    return (addrmode_ & kMemOperandRegisterOnly) != 0;
  }

  bool IsImmediate() const { return !rm_.IsValid(); }
  bool IsImmediateZero() const { return !rm_.IsValid() && (offset_ == 0); }
  bool IsPlainRegister() const {
    return rm_.IsValid() && shift_.IsLSL() && (shift_amount_ == 0);
  }
  bool IsShiftedRegister() const { return rm_.IsValid(); }
  bool IsImmediateOffset() const {
    return (GetAddrMode() == Offset) && !rm_.IsValid();
  }
  bool IsImmediateZeroOffset() const {
    return (GetAddrMode() == Offset) && !rm_.IsValid() && (offset_ == 0);
  }
  bool IsRegisterOffset() const {
    return (GetAddrMode() == Offset) && rm_.IsValid() && shift_.IsLSL() &&
           (shift_amount_ == 0);
  }
  bool IsShiftedRegisterOffset() const {
    return (GetAddrMode() == Offset) && rm_.IsValid();
  }
  uint32_t GetTypeEncodingValue() const {
    return shift_.IsRRX() ? kRRXEncodedValue : shift_.GetValue();
  }
  bool IsOffset() const { return GetAddrMode() == Offset; }
  bool IsPreIndex() const { return GetAddrMode() == PreIndex; }
  bool IsPostIndex() const { return GetAddrMode() == PostIndex; }
  bool IsShiftValid() const { return shift_.IsValidAmount(shift_amount_); }

 private:
  static const int kMemOperandRegisterOnly = 0x1000;
  static const int kMemOperandAddrModeMask = 0xfff;
  void CheckShift();
  Register rn_;
  int32_t offset_;
  Sign sign_;
  Register rm_;
  Shift shift_;
  uint32_t shift_amount_;
  uint32_t addrmode_;
};

// Operand represents generic set of arguments to pass to an instruction.
//
//   Usage: <instr> <Rd> , <Operand>
//
//   where <instr> is the instruction to use (e.g., Mov(), Rsb(), etc.)
//         <Rd> is the destination register
//         <Operand> is the rest of the arguments to the instruction
//
//   <Operand> can be one of:
//
//   #<imm> - an unsigned 32-bit immediate value
//   <Rm>, <shift> <#amount> - immediate shifted register
//   <Rm>, <shift> <Rs> - register shifted register
//
class Operand {
 public:
  // { #<immediate> }
  // where <immediate> is uint32_t.
  // This is allowed to be an implicit constructor because Operand is
  // a wrapper class that doesn't normally perform any type conversion.
  Operand(uint32_t immediate)  // NOLINT(runtime/explicit)
      : imm_(immediate), rm_(NoReg), shift_(LSL), amount_(0), rs_(NoReg) {}
  Operand(int32_t immediate)  // NOLINT(runtime/explicit)
      : imm_(immediate), rm_(NoReg), shift_(LSL), amount_(0), rs_(NoReg) {}

  // rm
  // where rm is the base register
  // This is allowed to be an implicit constructor because Operand is
  // a wrapper class that doesn't normally perform any type conversion.
  Operand(Register rm);

  // rm, <shift>
  // where rm is the base register, and
  //       <shift> is RRX
  Operand(Register rm, Shift shift);

  // rm, <shift> #<amount>
  // where rm is the base register, and
  //       <shift> is one of {LSL, LSR, ASR, ROR}, and
  //       <amount> is uint6_t.
  Operand(Register rm, Shift shift, uint32_t amount);

  // rm, <shift> rs
  // where rm is the base register, and
  //       <shift> is one of {LSL, LSR, ASR, ROR}, and
  //       rs is the shifted register
  Operand(Register rm, Shift shift, Register rs);

  bool IsImmediate() const { return !rm_.IsValid(); }

  bool IsPlainRegister() const {
    return rm_.IsValid() && !shift_.IsRRX() && !rs_.IsValid() && (amount_ == 0);
  }

  bool IsImmediateShiftedRegister() const {
    return rm_.IsValid() && !rs_.IsValid();
  }

  bool IsRegisterShiftedRegister() const {
    return rm_.IsValid() && rs_.IsValid();
  }

  uint32_t GetImmediate() const;

  int32_t GetSignedImmediate() const;

  Register GetBaseRegister() const;

  Shift GetShift() const;

  uint32_t GetShiftAmount() const;

  Register GetShiftRegister() const;

  uint32_t GetTypeEncodingValue() const {
    return shift_.IsRRX() ? kRRXEncodedValue : shift_.GetValue();
  }

 private:
// Forbid implicitly creating operands around types that cannot be encoded
// into a uint32_t without loss.
#if __cplusplus >= 201103L
  Operand(int64_t) = delete;
  Operand(uint64_t) = delete;
  Operand(float) = delete;
  Operand(double) = delete;
#else
  VIXL_NO_RETURN_IN_DEBUG_MODE Operand(int64_t);
  VIXL_NO_RETURN_IN_DEBUG_MODE Operand(uint64_t);
  VIXL_NO_RETURN_IN_DEBUG_MODE Operand(float);
  VIXL_NO_RETURN_IN_DEBUG_MODE Operand(double);
#endif

  uint32_t imm_;
  Register rm_;
  Shift shift_;
  uint32_t amount_;
  Register rs_;
};

struct Aarch32RawLiteral;
Aarch32RawLiteral* create_aarch32_raw_literal(const void* addr,
                                              int size,
                                              PlacementPolicy placement_policy,
                                              DeletionPolicy deletion_policy);
void destroy_aarch32_raw_literal(Aarch32RawLiteral* raw_literal);

struct Aarch32Label;
Aarch32Label* create_aarch32_label();
Aarch32Label* create_aarch32_label(int32_t offset);
void destroy_aarch32_label(Aarch32Label* label);

struct Aarch32DOperand;
Aarch32DOperand* create_aarch32_doperand();
void destroy_aarch32_doperand();

struct Aarch32QOperand;
Aarch32QOperand* create_aarch32_qoperand();
void destroy_aarch32_qoperand();

struct Aarch32SOperand;
Aarch32SOperand* create_aarch32_soperand();
void destroy_aarch32_soperand();

struct Aarch32MacroAssembler;
Aarch32MacroAssembler* create_aarch32_masm();
void masm_bind(Aarch32MacroAssembler* masm, Aarch32Label* label);
void masm_finalize(Aarch32MacroAssembler* masm);
const uint8_t* masm_get_start_address(const Aarch32MacroAssembler* masm);
uint32_t masm_get_size_of_code_generated(const Aarch32MacroAssembler* masm);
void destroy_aarch32_masm(Aarch32MacroAssembler* masm);

#endif
