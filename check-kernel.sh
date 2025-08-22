#!/usr/bin/env bash
# This file contain multiple sanity check for testing purpose of the
# vectors table alginement and such.
# run chmod +x ./check-kernel.sh before hand
set -e

ELF=build/kernel.elf

echo "=== Running sanity checks on $ELF ==="

# Check .vectors at 0x0
if arm-none-eabi-objdump -h "$ELF" | grep -q '\.vectors.*00000000'; then
  echo "[PASS] .vectors section is mapped at VMA 0x00000000"
else
  echo "[FAIL] .vectors not at 0x0!"
fi

# Check entry point is _start
ENTRY=$(arm-none-eabi-readelf -h "$ELF" | grep 'Entry point' | awk '{print $4}' | sed 's/^0x//')
START_ADDR=$(arm-none-eabi-nm -n "$ELF" | grep " T _start" | awk '{print $1}')
if [ $((16#$ENTRY)) -eq $((16#$START_ADDR)) ]; then
  echo "[PASS] Entry point matches _start (0x$ENTRY)"
else
  echo "[FAIL] Entry point mismatch: entry=0x$ENTRY, _start=0x$START_ADDR"
fi

# Check vector table branches correctly
DISASM=$(arm-none-eabi-objdump -d -j .vectors "$ELF")

if echo "$DISASM" | grep -q "b.*<_start>"; then
  echo "[PASS] Reset vector branches to _start"
else
  echo "[FAIL] Reset vector does not branch to _start!"
fi

if echo "$DISASM" | grep -q "b.*<irq_entry>"; then
  echo "[PASS] IRQ vector branches to irq_entry"
else
  echo "[FAIL] IRQ vector does not branch to irq_entry!"
fi

# Check that irq_entry symbol exists
if arm-none-eabi-nm -n "$ELF" | grep -q " T irq_entry"; then
  echo "[PASS] irq_entry symbol is defined"
else
  echo "[FAIL] irq_entry missing!"
fi

# Check BSS symbols exist
if arm-none-eabi-nm -n "$ELF" | grep -q "__bss_start" && \
   arm-none-eabi-nm -n "$ELF" | grep -q "__bss_end"; then
  echo "[PASS] __bss_start and __bss_end are defined"
else
  echo "[FAIL] Missing __bss_start/__bss_end!"
fi

echo "=== Checks complete ==="
