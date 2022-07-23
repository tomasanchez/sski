import os

os.system("rm -fr /home/tosanchez/Dev/sski/config")
os.system("cp -r /home/tosanchez/Dev/sski/test/suspension/config /home/tosanchez/Dev/sski")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/suspension/SUSPE_1.txt 20")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/suspension/SUSPE_2.txt 20")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/suspension/SUSPE_3.txt 10")
