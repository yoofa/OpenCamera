/*
 * alsasymboltable_linux.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "alsasymboltable_linux.h"

namespace ave {
namespace adm_linux_alsa {

LATE_BINDING_SYMBOL_TABLE_DEFINE_BEGIN(AlsaSymbolTable, "libasound.so.2")
#define X(sym) LATE_BINDING_SYMBOL_TABLE_DEFINE_ENTRY(AlsaSymbolTable, sym)
ALSA_SYMBOLS_LIST
#undef X
LATE_BINDING_SYMBOL_TABLE_DEFINE_END(AlsaSymbolTable)

}  // namespace adm_linux_alsa
}  // namespace ave
