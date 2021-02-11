import os

output = open("emotes.lib", "w")
files = [f for f in os.listdir('.') if os.path.isfile(f)]

for f in files:
    if f == "emotes.lib":
        continue
    output.write(os.path.splitext(f)[0])
    output.write(" ")
    output.write(f)
    output.write("\n")