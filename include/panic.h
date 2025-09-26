#pragma once

__attribute__((noreturn)) void kernel_halt(void);
void kernel_panic(const char *message);

