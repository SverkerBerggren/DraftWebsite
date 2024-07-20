import requests
import json
import sys
import os


def main():
    cardNames = [line.rstrip() for line in open(sys.argv[1]).readlines()]
    offset = 0
    outDir = sys.argv[2]
    os.makedirs(outDir,exist_ok=True)
    while offset < len(cardNames):
        newCardCount = min(len(cardNames)-offset,75)
        tempNames = cardNames[offset:(offset+newCardCount)]
        offset += newCardCount
        body = {"identifiers": [{"name": s} for s in tempNames]}
        print(body)
        res = requests.post("https://api.scryfall.com/cards/collection",json=body).json()
        CardImages = [ (tempNames[i],res["data"][i]["image_uris"]["normal"]) for i in range(0,len(tempNames))]
        for Card in CardImages:
            content = requests.get(Card[1]).content
            with open(f"{outDir}/{Card[0]}.jpg","wb") as OutFile:
                OutFile.write(content)
main()
