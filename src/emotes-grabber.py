import os

<<<<<<< HEAD
output = open("emotes.lib", "w")
files = [f for f in os.listdir('.') if os.path.isfile(f)]
=======
files = [f for f in os.listdir('.') if os.path.isfile(f)]
lib = open(r"emotes.lib", "w")
>>>>>>> master

for f in files:
    if f == "emotes.lib":
        continue
<<<<<<< HEAD
    output.write(os.path.splitext(f)[0])
    output.write(" ")
    output.write(f)
    output.write("\n")
    
=======
    lib.write(os.path.splitext(f)[0])
    lib.write(" ")
    lib.write(f)
    lib.write("\n")
>>>>>>> master
