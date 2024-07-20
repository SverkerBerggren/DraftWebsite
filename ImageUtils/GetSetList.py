import requests
import json
import sys
import os

def getCompleteList(res):
    returnValue = res["data"]
    if(res["has_more"]):
        returnValue.extend(getCompleteList(requests.get(res["next_page"]).json()))
    return returnValue
def main():
    setID = sys.argv[1]
    endPoint = f"https://api.scryfall.com/cards/search?q=set:{setID}"
    result = getCompleteList(requests.get(endPoint).json())
    for Card in result:
        print(Card["name"])
main()
