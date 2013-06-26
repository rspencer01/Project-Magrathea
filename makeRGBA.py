a = open(raw_input("Input file: "),"rb")
b = open(raw_input("Output file: "),"wb")
c = a.read()
d = ""
a.close()
for i in range(len(c)/3):
    if (c[i*3]!=chr(0)):
        d+=c[i*3]+c[i*3+1]+c[i*3+2]+chr(255)
    else:
        d+=c[i*3]+c[i*3+1]+c[i*3+2]+chr(0)
b.write(d)
b.close()
