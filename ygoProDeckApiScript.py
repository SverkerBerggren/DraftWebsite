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

with open("KortIKuben.txt") as textFile:
    for line in textFile: 
        cardId = line.rstrip()
        response = requests.get("https://db.ygoprodeck.com/api/v7/cardinfo.php?id=" + cardId)
        cardImageURL = json.dumps(response["data"][0]["card_images"][0]["image_url"])
        cardImageURL = cardImageURL[1:-1]
        cardImage = requests.get(cardImageURL).content
        with open( "" + cardId + ".jpg", 'wb') as handler:
            handler.write(cardImage)
        time.sleep(0.1)

#with open( "" + id + ".jpg", 'wb') as handler:
#    handler.write(image)

