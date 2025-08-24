#!/usr/bin/env python3
# Compact Analysis
import os,sys
def analyze(): print(f"Files: {len([f for f in os.listdir('.') if f.endswith('.xml')])}")
def test(): os.system("make -f Makefile-compact run-sim && make -f Makefile-compact run-classifier")
if __name__=="__main__": test() if len(sys.argv)>1 and sys.argv[1]=="test" else analyze()
