import os

files = [f for f in os.listdir('.') if os.path.isfile(f)]
lib = open(r"emotes.lib", "w")

for f in files:
    if f == "emotes.lib":
        continue
    lib.write(os.path.splitext(f)[0])
    lib.write(" ")
    lib.write(f)
    lib.write("\n")