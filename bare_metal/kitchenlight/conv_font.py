import sys

in_file = sys.argv[1]
out_file = sys.argv[2]

def line2bitfield(line):
    r = 0
    for i in xrange(len(line)):
        #v = 1 << (len(line) - 1 - i)
        v = 1 << i #lsb is left-most pixel
        if line[i] == '#':
            r |= v
    return r

def read_chars():
    chars = [ (5, [0]*6) ] * 256

    with open(in_file, 'rb') as f:
        lines = f.readlines()
        f.close()
    lines = [l[:-1] for l in lines]

    for i in xrange(0, len(lines), 7):
        index = ord(lines[i])
        width = len(lines[i+1])
        l = [line2bitfield(lines[j]) for j in xrange(i+1, i+7)]
        chars[index] = (width, l)

    return chars

def main():
    chars = read_chars()

    with open(out_file, 'wb') as f:
        f.write('#include "pixelfont.h"\nstruct Pixelfont std_font = {{\n')
        for width, bitfields in chars:
            f.write('{%d,{%d,%d,%d,%d,%d,%d}},\n' % ((width,) + tuple(bitfields)))
        f.write('}};\n')
        f.close()

main()
