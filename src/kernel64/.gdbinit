define tr
file kernel32.elf
target remote 127.0.0.1:1234
set arch i386
disp/i$pc
u *&kernel_main
end

tr
