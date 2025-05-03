#ifndef VIXL_AARCH32_WRAPPER_STRUCTS_AARCH32_H_
#define VIXL_AARCH32_WRAPPER_STRUCTS_AARCH32_H_

#include "aarch32/macro-assembler-aarch32.h"

struct Aarch32RawLiteral : vixl::aarch32::RawLiteral {
  Aarch32RawLiteral(const void* addr,
                    int size,
                    PlacementPolicy placement_policy,
                    DeletionPolicy deletion_policy)
      : vixl::aarch32::RawLiteral(addr,
                                  size,
                                  placement_policy,
                                  deletion_policy) {}
};

struct Aarch32Label : vixl::aarch32::Label {
  Aarch32Label() : vixl::aarch32::Label() {}
  Aarch32Label(int32_t offset) : vixl::aarch32::Label(offset) {}
};

struct Aarch32MacroAssembler : vixl::aarch32::MacroAssembler {
  Aarch32MacroAssembler(InstructionSet isa)
      : vixl::aarch32::MacroAssembler(isa) {}
};

struct Aarch32DOperand : vixl::aarch32::DOperand {
  Aarch32DOperand(int32_t immediate) : vixl::aarch32::DOperand(immediate) {}
  Aarch32DOperand(uint32_t immediate) : vixl::aarch32::DOperand(immediate) {}
  Aarch32DOperand(int64_t immediate) : vixl::aarch32::DOperand(immediate) {}
  Aarch32DOperand(uint64_t immediate) : vixl::aarch32::DOperand(immediate) {}
  Aarch32DOperand(float immediate) : vixl::aarch32::DOperand(immediate) {}
  Aarch32DOperand(double immediate) : vixl::aarch32::DOperand(immediate) {}
  Aarch32DOperand(const vixl::aarch32::NeonImmediate& imm)
      : vixl::aarch32::DOperand(imm) {}
  Aarch32DOperand(DRegister rm) : vixl::aarch32::DOperand(rm) {}
};

struct Aarch32QOperand : vixl::aarch32::QOperand {
  Aarch32QOperand(int32_t immediate) : vixl::aarch32::QOperand(immediate) {}
  Aarch32QOperand(uint32_t immediate) : vixl::aarch32::QOperand(immediate) {}
  Aarch32QOperand(int64_t immediate) : vixl::aarch32::QOperand(immediate) {}
  Aarch32QOperand(uint64_t immediate) : vixl::aarch32::QOperand(immediate) {}
  Aarch32QOperand(float immediate) : vixl::aarch32::QOperand(immediate) {}
  Aarch32QOperand(double immediate) : vixl::aarch32::QOperand(immediate) {}
  Aarch32QOperand(const vixl::aarch32::NeonImmediate& imm)
      : vixl::aarch32::QOperand(imm) {}
  Aarch32QOperand(QRegister rm) : vixl::aarch32::QOperand(rm) {}
};

struct Aarch32SOperand : vixl::aarch32::SOperand {
  Aarch32SOperand(int32_t immediate) : vixl::aarch32::SOperand(immediate) {}
  Aarch32SOperand(uint32_t immediate) : vixl::aarch32::SOperand(immediate) {}
  Aarch32SOperand(float immediate) : vixl::aarch32::SOperand(immediate) {}
  Aarch32SOperand(double immediate) : vixl::aarch32::SOperand(immediate) {}
  Aarch32SOperand(const vixl::aarch32::NeonImmediate& imm)
      : vixl::aarch32::SOperand(imm) {}
  Aarch32SOperand(SRegister rm) : vixl::aarch32::SOperand(rm) {}
};

#endif
