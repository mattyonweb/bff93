import sys, os, shutil
from PIL import Image, ImageDraw

W_RECT = 10
H_RECT = 10

where_am_i   = "/".join(os.path.abspath(__file__).split("/")[:-1]) + "/"
tests_folder = "/".join(where_am_i.split("/")[:-2]) + "/tests/"
out_folder   = "/".join(os.path.abspath(__file__).split("/")[:-2]) + "/out/"

#print(where_am_i)
#print(tests_folder)
#print(out_folder)

colors = {
    "+" : (255,162,142),    # arancionicci
    "-" :  	(255,151,128),
    "*" :  	(255,144,121),
    "/" :  	(255,152,105),
    "%" :  	(255,211,132),
    
    "!" : (161,0,32),       #rossicci
    "`" : (148,0,22),
    "?" :  	(135,0,20),
    "_" : (122,0,12),
    "|" : (111,17,26),
    
    ">" :  	(108,139,193),  #azzurrini
    "<" :  	(129,156,203),
    "^" : (150,172,211),
    "v" :  	(172,189,219),
    "#" :  	(193,205,229),

    "\"" : (136,246,101),   #verdi
    ":" :  	(143,236,140),
    "\\" : (168,212,134),
    "$" :  	(166,193,140),

    "." : (245,255,0),
    "," : (255,232,102),
    "&" : (245,255,0),
    "~" : (255,232,102),
    
    "p" : (76,26,129),
    "g" : (111,55,171),
    
    "@" : (0, 0, 0),
    " " : (255,255,255)
}

for i, c in enumerate(map(str, range(10))):
    colors[c] = (216-(7*i),216-(7*i),216-(7*i))
    
def fill_matrix(matrix):
    max_width = len(max(matrix, key=len))

    for i, row in enumerate(matrix):
        if len(row) < max_width:
            matrix[i] += " " * (max_width - len(row))
            
    return matrix

for fn in os.listdir(tests_folder):
    if fn[-2:] == "py": continue
    
    with open(tests_folder+fn, "r") as f:
        matrix = f.readlines()

    matrix = fill_matrix(matrix)
    width, height = len(max(matrix, key=len)), len(matrix)
        
    img = Image.new("RGB", (width * W_RECT, height * H_RECT), "white")
    draw = ImageDraw.Draw(img)
    
    for y,row in enumerate(matrix):
        for x,ch in enumerate(row):
            try:
                color = colors[ch]
            except:
                color = (255,255,255)
                
            draw.rectangle([x*W_RECT,y*H_RECT,W_RECT*(x+1),H_RECT*(y+1)], color, color)

    img.save(where_am_i + fn + ".jpg")
    shutil.move(where_am_i + fn + ".jpg", out_folder + fn + ".jpg")

print("Done!")
print("Output files in {}".format(out_folder))
