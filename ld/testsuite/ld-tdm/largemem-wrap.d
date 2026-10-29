#source: largemem-wrap.s
#ld: -m elf32tdm_largemem
#name: TDM .text larger than the address space
#error: .*section \.text VMA wraps around address space
