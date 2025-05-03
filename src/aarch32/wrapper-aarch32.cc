#include "aarch32/wrapper-aarch32.h"

#include "aarch32/macro-assembler-aarch32.h"
#include "aarch32/wrapper-aarch32-structs.h"

Condition::Condition(uint32_t condition) : condition_(condition) {
  VIXL_ASSERT(condition <= kNone);
}

Condition Condition::Negate() const {
  VIXL_ASSERT(IsNot(al) && IsNot(kNever));
  return Condition(condition_ ^ 1);
}

const char* Condition::GetName() const {
  switch (condition_) {
    case eq:
      return "eq";
    case ne:
      return "ne";
    case cs:
      return "cs";
    case cc:
      return "cc";
    case mi:
      return "mi";
    case pl:
      return "pl";
    case vs:
      return "vs";
    case vc:
      return "vc";
    case hi:
      return "hi";
    case ls:
      return "ls";
    case ge:
      return "ge";
    case lt:
      return "lt";
    case gt:
      return "gt";
    case le:
      return "le";
    case al:
      return "";
    case Condition::kNone:
      return "";
  }
  return "<und>";
}

bool Shift::IsValidAmount(uint32_t amount) const {
  switch (GetType()) {
    case LSL:
      return amount <= 31;
    case ROR:
      return (amount > 0) && (amount <= 31);
    case LSR:
    case ASR:
      return (amount > 0) && (amount <= 32);
    case RRX:
      return amount == 0;
    default:
      VIXL_UNREACHABLE();
      return false;
  }
}

const char* Shift::GetName() const {
  switch (shift_) {
    case LSL:
      return "lsl";
    case LSR:
      return "lsr";
    case ASR:
      return "asr";
    case ROR:
      return "ror";
    case RRX:
      return "rrx";
  }
  VIXL_UNREACHABLE();
  return "??";
}

CPURegister::CPURegister(RegisterType type, uint32_t code, int size)
    : value_((type << kTypeShift) | (code << kCodeShift) |
             (size << kSizeShift)) {
#ifdef VIXL_DEBUG
  switch (type) {
    case kNoRegister:
      break;
    case kRRegister:
      VIXL_ASSERT(code < kNumberOfRegisters);
      VIXL_ASSERT(size == kRegSizeInBits);
      break;
    case kSRegister:
      VIXL_ASSERT(code < kNumberOfSRegisters);
      VIXL_ASSERT(size == kSRegSizeInBits);
      break;
    case kDRegister:
      VIXL_ASSERT(code < kMaxNumberOfDRegisters);
      VIXL_ASSERT(size == kDRegSizeInBits);
      break;
    case kQRegister:
      VIXL_ASSERT(code < kNumberOfQRegisters);
      VIXL_ASSERT(size == kQRegSizeInBits);
      break;
    default:
      VIXL_UNREACHABLE();
      break;
  }
#endif
}

Register::Register(uint32_t code)
    : CPURegister(kRRegister, code % kNumberOfRegisters, kRegSizeInBits) {
  VIXL_ASSERT(GetCode() < kNumberOfRegisters);
}

SRegister DRegister::GetLane(uint32_t lane) const {
  uint32_t lane_count = kDRegSizeInBits / kSRegSizeInBits;
  VIXL_ASSERT(lane < lane_count);
  VIXL_ASSERT(GetCode() * lane_count < kNumberOfSRegisters);
  return SRegister(GetCode() * lane_count + lane);
}
uint32_t DRegister::Encode(int single_bit_field,
                           int four_bit_field_lowest_bit) const {
  VIXL_ASSERT(single_bit_field >= 4);
  return ((GetCode() & 0x10) << (single_bit_field - 4)) |
         ((GetCode() & 0xf) << four_bit_field_lowest_bit);
}

DRegister QRegister::GetDLane(uint32_t lane) const {
  uint32_t lane_count = kQRegSizeInBits / kDRegSizeInBits;
  VIXL_ASSERT(lane < lane_count);
  return DRegister(GetCode() * lane_count + lane);
}

SRegister QRegister::GetSLane(uint32_t lane) const {
  uint32_t lane_count = kQRegSizeInBits / kSRegSizeInBits;
  VIXL_ASSERT(lane < lane_count);
  VIXL_ASSERT(GetCode() * lane_count < kNumberOfSRegisters);
  return SRegister(GetCode() * lane_count + lane);
}

uint32_t QRegister::Encode(int single_bit_field,
                           int four_bit_field_lowest_bit) {
  // Encode "code * 2".
  VIXL_ASSERT(single_bit_field >= 3);
  return ((GetCode() & 0x8) << (single_bit_field - 3)) |
         ((GetCode() & 0x7) << (four_bit_field_lowest_bit + 1));
}

uint32_t DRegisterLane::EncodeX(DataType dt,
                                int single_bit_field,
                                int four_bit_field_lowest_bit) const {
  VIXL_ASSERT(single_bit_field >= 4);
  uint32_t value = lane_ << ((dt.GetSize() == 16) ? 3 : 4) | GetCode();
  return ((value & 0x10) << (single_bit_field - 4)) |
         ((value & 0xf) << four_bit_field_lowest_bit);
}

SRegister VRegister::S() const {
  VIXL_ASSERT(GetType() == kSRegister);
  return SRegister(GetCode());
}


DRegister VRegister::D() const {
  VIXL_ASSERT(GetType() == kDRegister);
  return DRegister(GetCode());
}


QRegister VRegister::Q() const {
  VIXL_ASSERT(GetType() == kQRegister);
  return QRegister(GetCode());
}

bool RegisterList::IsSingleRegister() const { return vixl::IsPowerOf2(list_); }

int RegisterList::GetCount() const { return vixl::CountSetBits(list_); }

Register RegisterList::GetFirstAvailableRegister() const {
  if (list_ == 0) {
    return Register();
  }
  return Register(vixl::CountTrailingZeros(list_));
}

NeonRegisterList::NeonRegisterList(DRegister reg, TransferType type)
    : first_(reg.GetCode()),
      spacing_(kSingle),
      type_(type),
      lane_(-1),
      length_(1) {
  VIXL_ASSERT(type_ != kOneLane);
}
NeonRegisterList::NeonRegisterList(DRegister reg, int lane)
    : first_(reg.GetCode()),
      spacing_(kSingle),
      type_(kOneLane),
      lane_(lane),
      length_(1) {
  VIXL_ASSERT((lane_ >= 0) && (lane_ < 8));
}
NeonRegisterList::NeonRegisterList(DRegister first,
                                   DRegister last,
                                   SpacingType spacing,
                                   TransferType type)
    : first_(first.GetCode()), spacing_(spacing), type_(type), lane_(-1) {
  VIXL_ASSERT(type != kOneLane);
  VIXL_ASSERT(first.GetCode() <= last.GetCode());

  int range = last.GetCode() - first.GetCode();
  VIXL_ASSERT(IsSingleSpaced() || vixl::IsMultiple(range, 2));
  length_ = (IsDoubleSpaced() ? (range / 2) : range) + 1;

  VIXL_ASSERT(length_ <= 4);
}
NeonRegisterList::NeonRegisterList(DRegister first,
                                   DRegister last,
                                   SpacingType spacing,
                                   int lane)
    : first_(first.GetCode()), spacing_(spacing), type_(kOneLane), lane_(lane) {
  VIXL_ASSERT((lane >= 0) && (lane < 8));
  VIXL_ASSERT(first.GetCode() <= last.GetCode());

  int range = last.GetCode() - first.GetCode();
  VIXL_ASSERT(IsSingleSpaced() || vixl::IsMultiple(range, 2));
  length_ = (IsDoubleSpaced() ? (range / 2) : range) + 1;

  VIXL_ASSERT(length_ <= 4);
}
DRegister NeonRegisterList::GetDRegister(int n) const {
  VIXL_ASSERT(n >= 0);
  VIXL_ASSERT(n < length_);
  unsigned code = first_.GetCode() + (IsDoubleSpaced() ? (2 * n) : n);
  VIXL_ASSERT(code < kMaxNumberOfDRegisters);
  return DRegister(code);
}

DRegisterList::DRegisterList(DRegister first, int length)
    : first_(first.GetCode()), length_(length) {
  VIXL_ASSERT(length >= 0);
}

DRegister DRegisterList::GetDRegister(int n) const {
  VIXL_ASSERT(n >= 0);
  VIXL_ASSERT(n < length_);
  return DRegister((first_.GetCode() + n) % kMaxNumberOfDRegisters);
}

const char* SpecialRegister::GetName() const {
  switch (reg_) {
    case APSR:
      return "APSR";
    case SPSR:
      return "SPSR";
  }
  VIXL_UNREACHABLE();
  return "??";
}

const char* MaskedSpecialRegister::GetName() const {
  switch (reg_) {
    case APSR_nzcvq:
      return "APSR_nzcvq";
    case APSR_g:
      return "APSR_g";
    case APSR_nzcvqg:
      return "APSR_nzcvqg";
    case CPSR_c:
      return "CPSR_c";
    case CPSR_x:
      return "CPSR_x";
    case CPSR_xc:
      return "CPSR_xc";
    case CPSR_sc:
      return "CPSR_sc";
    case CPSR_sx:
      return "CPSR_sx";
    case CPSR_sxc:
      return "CPSR_sxc";
    case CPSR_fc:
      return "CPSR_fc";
    case CPSR_fx:
      return "CPSR_fx";
    case CPSR_fxc:
      return "CPSR_fxc";
    case CPSR_fsc:
      return "CPSR_fsc";
    case CPSR_fsx:
      return "CPSR_fsx";
    case CPSR_fsxc:
      return "CPSR_fsxc";
    case SPSR_c:
      return "SPSR_c";
    case SPSR_x:
      return "SPSR_x";
    case SPSR_xc:
      return "SPSR_xc";
    case SPSR_s:
      return "SPSR_s";
    case SPSR_sc:
      return "SPSR_sc";
    case SPSR_sx:
      return "SPSR_sx";
    case SPSR_sxc:
      return "SPSR_sxc";
    case SPSR_f:
      return "SPSR_f";
    case SPSR_fc:
      return "SPSR_fc";
    case SPSR_fx:
      return "SPSR_fx";
    case SPSR_fxc:
      return "SPSR_fxc";
    case SPSR_fs:
      return "SPSR_fs";
    case SPSR_fsc:
      return "SPSR_fsc";
    case SPSR_fsx:
      return "SPSR_fsx";
    case SPSR_fsxc:
      return "SPSR_fsxc";
  }
  VIXL_UNREACHABLE();
  return "??";
}

MaskedSpecialRegister::MaskedSpecialRegister(uint32_t reg) : reg_(reg) {
  VIXL_ASSERT(reg <= SPSR_fsxc);
}

SRegisterList::SRegisterList(SRegister first, int length)
    : first_(first.GetCode()), length_(length) {
  VIXL_ASSERT(length >= 0);
}
SRegister SRegisterList::GetSRegister(int n) const {
  VIXL_ASSERT(n >= 0);
  VIXL_ASSERT(n < length_);
  return SRegister((first_.GetCode() + n) % kNumberOfSRegisters);
}

SpecialFPRegister::SpecialFPRegister(uint32_t reg) : reg_(reg) {
#ifdef VIXL_DEBUG
  switch (reg) {
    case FPSID:
    case FPSCR:
    case MVFR2:
    case MVFR1:
    case MVFR0:
    case FPEXC:
      break;
    default:
      VIXL_UNREACHABLE();
  }
#endif
}

const char* SpecialFPRegister::GetName() const {
  switch (reg_) {
    case FPSID:
      return "FPSID";
    case FPSCR:
      return "FPSCR";
    case MVFR2:
      return "MVFR2";
    case MVFR1:
      return "MVFR1";
    case MVFR0:
      return "MVFR0";
    case FPEXC:
      return "FPEXC";
  }
  VIXL_UNREACHABLE();
  return "??";
}

DataType::DataType(uint32_t size)
    : value_(static_cast<DataTypeValue>(kDataTypeUntyped | size)) {
  VIXL_ASSERT((size == 8) || (size == 16) || (size == 32) || (size == 64));
}

const char* DataType::GetName() const {
  switch (value_) {
    case kDataTypeValueInvalid:
      return ".??";
    case kDataTypeValueNone:
      return "";
    case S8:
      return ".s8";
    case S16:
      return ".s16";
    case S32:
      return ".s32";
    case S64:
      return ".s64";
    case U8:
      return ".u8";
    case U16:
      return ".u16";
    case U32:
      return ".u32";
    case U64:
      return ".u64";
    case F16:
      return ".f16";
    case F32:
      return ".f32";
    case F64:
      return ".f64";
    case I8:
      return ".i8";
    case I16:
      return ".i16";
    case I32:
      return ".i32";
    case I64:
      return ".i64";
    case P8:
      return ".p8";
    case P64:
      return ".p64";
    case Untyped8:
      return ".8";
    case Untyped16:
      return ".16";
    case Untyped32:
      return ".32";
    case Untyped64:
      return ".64";
  }
  VIXL_UNREACHABLE();
  return ".??";
}

MemoryBarrier::MemoryBarrier(uint32_t type)
    : type_(static_cast<MemoryBarrierType>(type)) {
  VIXL_ASSERT((type & 0x3) != 0);
}

const char* MemoryBarrier::GetName() const {
  switch (type_) {
    case OSHLD:
      return "oshld";
    case OSHST:
      return "oshst";
    case OSH:
      return "osh";
    case NSHLD:
      return "nshld";
    case NSHST:
      return "nshst";
    case NSH:
      return "nsh";
    case ISHLD:
      return "ishld";
    case ISHST:
      return "ishst";
    case ISH:
      return "ish";
    case LD:
      return "ld";
    case ST:
      return "st";
    case SY:
      return "sy";
  }
  switch (static_cast<int>(type_)) {
    case 0:
      return "#0x0";
    case 4:
      return "#0x4";
    case 8:
      return "#0x8";
    case 0xc:
      return "#0xc";
  }
  VIXL_UNREACHABLE();
  return "??";
}

RegisterOrAPSR_nzcv::RegisterOrAPSR_nzcv(uint32_t code) : code_(code) {
  VIXL_ASSERT(code_ < kNumberOfRegisters);
}

Register RegisterOrAPSR_nzcv::AsRegister() const {
  VIXL_ASSERT(!IsAPSR_nzcv());
  return Register(code_);
}

int32_t Sign::ApplyTo(uint32_t value) {
  return IsPlus() ? value : vixl::UnsignedNegate(value);
}

MemOperand::MemOperand(Register rn, AddrMode addrmode)
    : rn_(rn),
      offset_(0),
      sign_(plus),
      rm_(NoReg),
      shift_(LSL),
      shift_amount_(0),
      addrmode_(addrmode | kMemOperandRegisterOnly) {
  VIXL_ASSERT(rn_.IsValid());
}
MemOperand::MemOperand(Register rn, int32_t offset, AddrMode addrmode)
    : rn_(rn),
      offset_(offset),
      sign_((offset < 0) ? minus : plus),
      rm_(NoReg),
      shift_(LSL),
      shift_amount_(0),
      addrmode_(addrmode) {
  VIXL_ASSERT(rn_.IsValid());
}
MemOperand::MemOperand(Register rn,
                       Sign sign,
                       int32_t offset,
                       AddrMode addrmode)
    : rn_(rn),
      offset_(sign.IsPlus() ? offset : -offset),
      sign_(sign),
      rm_(NoReg),
      shift_(LSL),
      shift_amount_(0),
      addrmode_(addrmode) {
  VIXL_ASSERT(rn_.IsValid());
  // With this constructor, the sign must only be specified by "sign".
  VIXL_ASSERT(offset >= 0);
}
MemOperand::MemOperand(Register rn, Sign sign, Register rm, AddrMode addrmode)
    : rn_(rn),
      offset_(0),
      sign_(sign),
      rm_(rm),
      shift_(LSL),
      shift_amount_(0),
      addrmode_(addrmode) {
  VIXL_ASSERT(rn_.IsValid() && rm_.IsValid());
}
MemOperand::MemOperand(Register rn, Register rm, AddrMode addrmode)
    : rn_(rn),
      offset_(0),
      sign_(plus),
      rm_(rm),
      shift_(LSL),
      shift_amount_(0),
      addrmode_(addrmode) {
  VIXL_ASSERT(rn_.IsValid() && rm_.IsValid());
}
MemOperand::MemOperand(
    Register rn, Sign sign, Register rm, Shift shift, AddrMode addrmode)
    : rn_(rn),
      offset_(0),
      sign_(sign),
      rm_(rm),
      shift_(shift),
      shift_amount_(0),
      addrmode_(addrmode) {
  VIXL_ASSERT(rn_.IsValid() && rm_.IsValid());
  VIXL_ASSERT(shift_.IsRRX());
}
MemOperand::MemOperand(Register rn, Register rm, Shift shift, AddrMode addrmode)
    : rn_(rn),
      offset_(0),
      sign_(plus),
      rm_(rm),
      shift_(shift),
      shift_amount_(0),
      addrmode_(addrmode) {
  VIXL_ASSERT(rn_.IsValid() && rm_.IsValid());
  VIXL_ASSERT(shift_.IsRRX());
}
MemOperand::MemOperand(Register rn,
                       Sign sign,
                       Register rm,
                       Shift shift,
                       uint32_t shift_amount,
                       AddrMode addrmode)
    : rn_(rn),
      offset_(0),
      sign_(sign),
      rm_(rm),
      shift_(shift),
      shift_amount_(shift_amount),
      addrmode_(addrmode) {
  VIXL_ASSERT(rn_.IsValid() && rm_.IsValid());
  CheckShift();
}
MemOperand::MemOperand(Register rn,
                       Register rm,
                       Shift shift,
                       uint32_t shift_amount,
                       AddrMode addrmode)
    : rn_(rn),
      offset_(0),
      sign_(plus),
      rm_(rm),
      shift_(shift),
      shift_amount_(shift_amount),
      addrmode_(addrmode) {
  VIXL_ASSERT(rn_.IsValid() && rm_.IsValid());
  CheckShift();
}

void MemOperand::CheckShift() {
#ifdef VIXL_DEBUG
  // Disallow any zero shift other than RRX #0 and LSL #0 .
  if ((shift_amount_ == 0) && shift_.IsRRX()) return;
  if ((shift_amount_ == 0) && !shift_.IsLSL()) {
    VIXL_ABORT_WITH_MSG(
        "A shift by 0 is only accepted in "
        "the case of lsl and will be treated as "
        "no shift.\n");
  }
  switch (shift_.GetType()) {
    case LSL:
      VIXL_ASSERT(shift_amount_ <= 31);
      break;
    case ROR:
      VIXL_ASSERT(shift_amount_ <= 31);
      break;
    case LSR:
    case ASR:
      VIXL_ASSERT(shift_amount_ <= 32);
      break;
    case RRX:
    default:
      VIXL_UNREACHABLE();
      break;
  }
#endif
}

Operand::Operand(uint32_t immediate)
    : imm_(immediate), rm_(NoReg), shift_(LSL), amount_(0), rs_(NoReg) {}

Operand::Operand(int32_t immediate)
    : imm_(immediate), rm_(NoReg), shift_(LSL), amount_(0), rs_(NoReg) {}

Operand::Operand(Register rm)
    : imm_(0), rm_(rm), shift_(LSL), amount_(0), rs_(NoReg) {
  VIXL_ASSERT(rm_.IsValid());
}

Operand::Operand(Register rm, Shift shift)
    : imm_(0), rm_(rm), shift_(shift), amount_(0), rs_(NoReg) {
  VIXL_ASSERT(rm_.IsValid());
  VIXL_ASSERT(shift_.IsRRX());
}

Operand::Operand(Register rm, Shift shift, uint32_t amount)
    : imm_(0), rm_(rm), shift_(shift), amount_(amount), rs_(NoReg) {
  VIXL_ASSERT(rm_.IsValid());
  VIXL_ASSERT(!shift_.IsRRX());
#ifdef VIXL_DEBUG
  switch (shift_.GetType()) {
    case LSL:
      VIXL_ASSERT(amount_ <= 31);
      break;
    case ROR:
      VIXL_ASSERT(amount_ <= 31);
      break;
    case LSR:
    case ASR:
      VIXL_ASSERT(amount_ <= 32);
      break;
    case RRX:
    default:
      VIXL_UNREACHABLE();
      break;
  }
#endif
}

Operand::Operand(Register rm, Shift shift, Register rs)
    : imm_(0), rm_(rm), shift_(shift), amount_(0), rs_(rs) {
  VIXL_ASSERT(rm_.IsValid() && rs_.IsValid());
  VIXL_ASSERT(!shift_.IsRRX());
}

uint32_t Operand::GetImmediate() const {
  VIXL_ASSERT(IsImmediate());
  return imm_;
}

int32_t Operand::GetSignedImmediate() const {
  VIXL_ASSERT(IsImmediate());
  int32_t result;
  memcpy(&result, &imm_, sizeof(result));
  return result;
}

Register Operand::GetBaseRegister() const {
  VIXL_ASSERT(IsImmediateShiftedRegister() || IsRegisterShiftedRegister());
  return rm_;
}

Shift Operand::GetShift() const {
  VIXL_ASSERT(IsImmediateShiftedRegister() || IsRegisterShiftedRegister());
  return shift_;
}

uint32_t Operand::GetShiftAmount() const {
  VIXL_ASSERT(IsImmediateShiftedRegister());
  return amount_;
}

Register Operand::GetShiftRegister() const {
  VIXL_ASSERT(IsRegisterShiftedRegister());
  return rs_;
}

#if __cplusplus < 201103L
VIXL_NO_RETURN_IN_DEBUG_MODE Operand::Operand(int64_t) { VIXL_UNREACHABLE(); }
VIXL_NO_RETURN_IN_DEBUG_MODE Operand::Operand(uint64_t) { VIXL_UNREACHABLE(); }
VIXL_NO_RETURN_IN_DEBUG_MODE Operand::Operand(float) { VIXL_UNREACHABLE(); }
VIXL_NO_RETURN_IN_DEBUG_MODE Operand::Operand(double) { VIXL_UNREACHABLE(); }
#endif

Aarch32RawLiteral* create_aarch32_raw_literal(const void* addr,
                                              int size,
                                              PlacementPolicy placement_policy,
                                              DeletionPolicy deletion_policy) {
  return new Aarch32RawLiteral(addr, size, placement_policy, deletion_policy);
}
void destroy_aarch32_raw_literal(Aarch32RawLiteral* raw_literal) {
  delete raw_literal;
}

Aarch32Label* create_aarch32_label() { return new Aarch32Label(); }
Aarch32Label* create_aarch32_label(int32_t offset) {
  return new Aarch32Label(offset);
}
void destroy_aarch32_label(Aarch32Label* label) { delete label; }

Aarch32MacroAssembler* create_aarch32_masm(InstructionSet isa) {
  return new Aarch32MacroAssembler(isa);
}
void masm_bind(Aarch32MacroAssembler* masm, Aarch32Label* label) {
  masm->Bind(label);
}
void masm_finalize(Aarch32MacroAssembler* masm) { masm->FinalizeCode(); }
uint32_t masm_get_cursor_offset(Aarch32MacroAssembler* masm) {
  return masm->GetCursorOffset();
}
const uint8_t* masm_get_start_address(const Aarch32MacroAssembler* masm) {
  return masm->GetBuffer().GetStartAddress<const uint8_t*>();
}
uint32_t masm_get_size_of_code_generated(const Aarch32MacroAssembler* masm) {
  return masm->GetSizeOfCodeGenerated();
}
void destroy_aarch32_masm(Aarch32MacroAssembler* masm) { delete masm; }
