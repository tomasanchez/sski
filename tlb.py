import os

os.system("rm -fr /home/tosanchez/Dev/sski/config")
os.system("cp -r /home/tosanchez/Dev/sski/test/tlb/config /home/tosanchez/Dev/sski")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/tlb/TLB_1.txt 2048")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/tlb/TLB_2.txt 2048")
