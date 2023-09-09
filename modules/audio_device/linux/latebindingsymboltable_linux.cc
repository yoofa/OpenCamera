/*
 * latebindingsymboltable_linux.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "latebindingsymboltable_linux.h"

#include "base/logging.h"

#ifdef AVP_LINUX
#include <dlfcn.h>
#endif

namespace avp {
namespace adm_linux {

inline static const char* GetDllError() {
#ifdef AVP_LINUX
  char* err = dlerror();
  if (err) {
    return err;
  } else {
    return "No error";
  }
#else
#error Not implemented
#endif
}

DllHandle InternalLoadDll(const char dll_name[]) {
#ifdef AVP_LINUX
  DllHandle handle = dlopen(dll_name, RTLD_NOW);
#else
#error Not implemented
#endif
  if (handle == kInvalidDllHandle) {
    LOG(LS_WARNING) << "Can't load " << dll_name << " : " << GetDllError();
  }
  return handle;
}

void InternalUnloadDll(DllHandle handle) {
#ifdef AVP_LINUX
// TODO(pbos): Remove this dlclose() exclusion when leaks and suppressions from
// here are gone (or AddressSanitizer can display them properly).
//
// Skip dlclose() on AddressSanitizer as leaks including this module in the
// stack trace gets displayed as <unknown module> instead of the actual library
// -> it can not be suppressed.
// https://code.google.com/p/address-sanitizer/issues/detail?id=89
#if !defined(ADDRESS_SANITIZER)
  if (dlclose(handle) != 0) {
    LOG(LS_ERROR) << GetDllError();
  }
#endif  // !defined(ADDRESS_SANITIZER)
#else
#error Not implemented
#endif
}

static bool LoadSymbol(DllHandle handle,
                       const char* symbol_name,
                       void** symbol) {
#ifdef AVP_LINUX
  *symbol = dlsym(handle, symbol_name);
  char* err = dlerror();
  if (err) {
    LOG(LS_ERROR) << "Error loading symbol " << symbol_name << " : " << err;
    return false;
  } else if (!*symbol) {
    LOG(LS_ERROR) << "Symbol " << symbol_name << " is NULL";
    return false;
  }
  return true;
#else
#error Not implemented
#endif
}

// This routine MUST assign SOME value for every symbol, even if that value is
// NULL, or else some symbols may be left with uninitialized data that the
// caller may later interpret as a valid address.
bool InternalLoadSymbols(DllHandle handle,
                         int num_symbols,
                         const char* const symbol_names[],
                         void* symbols[]) {
#ifdef AVP_LINUX
  // Clear any old errors.
  dlerror();
#endif
  for (int i = 0; i < num_symbols; ++i) {
    if (!LoadSymbol(handle, symbol_names[i], &symbols[i])) {
      return false;
    }
  }
  return true;
}

}  // namespace adm_linux
}  // namespace avp
