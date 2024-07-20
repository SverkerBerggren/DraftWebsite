import requests
import json
import sys
import os


def main():
    cardNames = [line.rstrip() for line in open(sys.argv[1]).readlines()]
    offset = 0
    outDir = sys.argv[2]
    os.makedirs(outDir,exist_ok=True)
    splitRarity = "--split-rarity" in sys.argv
    while offset < len(cardNames):
        newCardCount = min(len(cardNames)-offset,75)
        tempNames = cardNames[offset:(offset+newCardCount)]
        offset += newCardCount
        body = {"identifiers": [{"name": s} for s in tempNames]}
        print(body)
        res = requests.post("https://api.scryfall.com/cards/collection",json=body).json()
        CardImages = [ (tempNames[i],res["data"][i]["image_uris"]["normal"], res["data"][i]["rarity"]) for i in range(0,len(tempNames))]
        for Card in CardImages:
            content = requests.get(Card[1]).content
            filePath = ""
            if not splitRarity:
                filePath = f"{outDir}/{Card[0]}.jpg"
            else:
                filePath = f"{outDir}/{Card[2]}/{Card[0]}.jpg"
            os.makedirs(os.path.dirname(filePath),exist_ok=True)
            with open(filePath,"wb") as OutFile:
                OutFile.write(content)
main()
