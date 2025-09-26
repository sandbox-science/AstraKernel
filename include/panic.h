#pragma once

void kernel_halt(void) __attribute__((noreturn));
void kernel_panic(const char *message);

