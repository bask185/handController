import pyperclip

s = pyperclip.paste()
#pyperclip.copy(s)
    
array_in =  [ [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0] ]

array_out = [ [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0] ]
            

# the type of s is string

lines= s.split('\r\n')

name = lines[1]
#print( 'name ' + name )
for i, line in enumerate (lines) :
    if i >= 3 and i <= 10 :
        newLine = line[2:]
        
        for j in range (0,8) :
            array_in[i-3][j] = newLine[j*2]

for i in range (0,8) :
    for j in range (0,8) :
        array_out[i][j] = array_in[7-j][i]


for i in range (0,8) :
    for j in range (0,8) :

        if array_out[i][j] == ' ' :
            array_out[i][j] = '0'

        elif array_out[i][j] == 'X' or  array_out[i][j] == 'x':
            array_out[i][j] = '1'

clipboard = ""

clipboard += 'const char '+ name +'[] /*PROGMEM*/  =\r\n'
clipboard += '{\r\n'
byte = ''
for i in range (0,8) :
    for j in range (0,8) :
        byte += array_out[i][j]

    clipboard += '    0b'+ byte + ',\r\n'
    byte = ''
clipboard+= '} ;'

print ( clipboard )

pyperclip.copy(clipboard)
#input('\r\n\r\npress enter')