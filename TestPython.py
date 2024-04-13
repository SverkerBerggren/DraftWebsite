import requests
import json
import urllib
import time

hej = 0
with open("KortIKuben.txt") as textFile:
    for line in textFile: 
        hej +=1

print(hej)