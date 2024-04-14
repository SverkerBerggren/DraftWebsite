import requests
import json
import urllib
import time

#testResponse = requests.get("https://db.ygoprodeck.com/api/v7/cardinfo.php?id=11074235")

#response = json.loads(testResponse.text)

#id =  json.dumps( response["data"][0]["card_images"][0]["id"], indent=2)
#imageResource =  json.dumps( response["data"][0]["card_images"][0]["image_url"], indent=2)

#imageResource = imageResource[1:-1]

#image = requests.get(imageResource).content

currentIteration = 0
currentCardId = ""
cardId = ""

try:
    with open("KortExtraDeck.TXT") as textFile:
        for line in textFile: 
            currentIteration +=1
            cardId = line.rstrip()
            response = json.loads(requests.get("https://db.ygoprodeck.com/api/v7/cardinfo.php?id=" + cardId).text)
            cardImageURL = json.dumps(response["data"][0]["card_images"][0]["image_url"])
            cardImageURL = cardImageURL[1:-1]
            cardImage = requests.get(cardImageURL).content
            with open( "D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML\\CardImages\\ExtraDeck\\" + cardId + ".jpg", 'wb') as handler:
                handler.write(cardImage)
        time.sleep(0.1)
except:
    print(cardId)
    print(currentIteration)
    print("det knasade")

print(currentIteration)
print("lyckades utan probelm")
#with open( "" + id + ".jpg", 'wb') as handler:
#    handler.write(image)

