import os

os.system("rm -fr /home/tosanchez/Dev/sski/config")
os.system("cp -r /home/tosanchez/Dev/sski/test/memory/config /home/tosanchez/Dev/sski")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/memory/MEMORIA_1.txt 2048")
