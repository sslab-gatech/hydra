from pwn import *

elf = ELF('./fsfuzz')
symbols = elf.symbols
sorted_symbols = {}

lines = open('log1', 'r').readlines()
for l in lines:
  addr = int(l.strip(), 16) - 0x0000555555554000
  for key, value in sorted(symbols.iteritems(), key=lambda (k,v): (v,k)):
    if addr > value:
      # print '%s: %s' % (key, hex(value))
      print '%s %s' % (key, hex(addr))
      break


