define tr
target remote 127.0.0.1:1234
set arch i8
disp/i $eip+$cs*16
u *0x7e06
end
