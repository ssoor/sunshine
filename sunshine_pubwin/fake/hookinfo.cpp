#include "hookinfo.h"

rule::_rule rule::rHookRule[rule::MaxTypeNumber] = { 0 };
hook::_module_hookinfo  hook::tHooks[hook::MaxModuleNumber] = { 0 };