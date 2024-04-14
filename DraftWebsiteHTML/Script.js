

let draftableCardSection = document.getElementById("draftableCardsSection");

let draftedCardsSection = document.getElementById("DraftedCardContainer");

let cardHighlight = document.getElementById("HighLightCard");

let downloadButton = document.getElementById("Downloadbutton");

let hostButton = document.getElementById("HostButton");
let startButton = document.getElementById("StartButton");

let startForm = document.getElementById("StartForm");

let joinButton = document.getElementById("JoinButton");
let joinInput = document.getElementById("InputFieldJoin");

let lobbyIdText = document.getElementById("lobbyIdText");

let inputCardsPerPack = document.getElementById("InputFieldCardPerPack");
let inputAmountOfPacks = document.getElementById("InputFieldAmountOfPack");
let inputAmountOfExtraDeckCards = document.getElementById("InputFieldAmountOfPlayers");

let playersJoinedArea = document.getElementById("PlayersJoined");

let startLobbyButton = document.getElementById("startLobbyButton");

HideHighlightCard();

joinButton.onclick = JoinLobby;

let joinedLobbyId ="";

let hostedLobbyId = "";

const DATA_INDEX = "data-index";


const controller = new AbortController();
const { signal } = controller;


// const draftedCards = [];

let continueDraftableCardsLoop = true;
let continuePlayerUpdate = true;
//AddAvailableCards();

let currentDraftableCards = [];
hostButton.onclick = HostLobby;


//ShowPack(simulatedPacks[0]);

//HostLobby();

continueHasLobbyStarted = true;
downloadButton.hidden = true;

startLobbyButton.onclick = StartLobby;



async function HostLobby()
{
    amountOfPacks = inputAmountOfPacks.value;
    mainDeckCardsPerPack = inputCardsPerPack.value;
    extraDeckCardsPerPack = inputAmountOfExtraDeckCards.value;
    jsonMessage = {amountOfPacks: amountOfPacks, mainDeckCardsPerPack: mainDeckCardsPerPack, extraDeckCardsPerPack: extraDeckCardsPerPack}

    

    startLobbyButton.hidden = false;
    startForm.remove();
    response = await fetch("/HostLobby",{
        method: "Post",
        body: JSON.stringify(jsonMessage) 
    }).then((response) => response.json()).then((json) =>{
        
        if(json["Accepted"])
        {
            hostedLobbyId = json["LobbyId"];
            lobbyIdText.textContent = "Lobby id: " + json["LobbyId"];
        }

       // console.log(json);
    });



    UpdateJoinedPlayers();

    UpdateDraftableCardsLoop();
}

async function JoinLobby()
{
    response = await fetch("/JoinLobby",{
        method: "Post",
        body: joinInput.value
        }).then((response) => response.text()).then((text) => {
            if(text == "Accepted")
            {
                startForm.remove();
                UpdateJoinedPlayers();
                UpdateLobbyStarted();
            }

            //console.log(text);
          });
}

async function StartLobby()
{
    //jsonMessage = JSON.parse("");



    response = await fetch("/StartLobby",{
        method: "Post"
        
    }).then((response) => response.text()).then((text) =>{
        
        //console.log(text);
    });
    
    //UpdateLoop();
}

async function UpdateDraftableCardsLoop()
{
    while(continueDraftableCardsLoop)
    {
        shouldEndDraft = false;
        response = await fetch("/Update",{
            method: "Post",
            signal: AbortSignal.timeout(15000),
            body : "UpdateDraftableCards"
            }).then((response) => response.json()   ).then((json) => {
                

                //console.log(json);
               // console.log(currentDraftableCards);
                if(json["DraftFinished"] == true)
                {
                    FinishDraftAndShowCards();
                    continueDraftableCardsLoop = false;
                    shouldEndDraft = true;
              
                }
                else
                {
                    if( (!arraysEqual(json["DraftableCards"], currentDraftableCards)) || currentDraftableCards.length == 0)
                    {
                        DraftableCardsFromServer(json["DraftableCards"]);
                    }
                    currentDraftableCards = json["DraftableCards"];
                }

              });

        await sleep(1000);
    }
}
async function UpdateJoinedPlayers()
{
    while(continuePlayerUpdate)
    {
        response = await fetch("/Update",{
            method: "Post",
            signal: AbortSignal.timeout(15000),
            body: "ConnectedPlayers"
            }).then((response) => response.json()).then((json) => {
                
                message = json["ConnectedPlayers"];

                RemoveChildren(playersJoinedArea);

                for(i = 0; i < message.length; i++)
                {
                    let paragraph = document.createElement("p");
                    paragraph.textContent = message[i];
                    playersJoinedArea.append(paragraph);
                }
                console.log(message);
              });

        await sleep(3000);
    }
}
async function UpdateLobbyStarted()
{
    while(continueHasLobbyStarted)
    {
        response = await fetch("/Update",{
            method: "Post",
            signal: AbortSignal.timeout(15000),
            body: "HasLobbyStarted"
            }).then((response) => response.json()).then((json) => {
                
                if(json["HasLobbyStarted"])
                {
                    UpdateDraftableCardsLoop();
                    continueHasLobbyStarted = false;
                }
              });

        await sleep(3000);
    }
}

//snodd kod
function arraysEqual(a, b) {
    if (a === b) return true;
    if (a == null || b == null) return false;
    if (a.length !== b.length) return false;
  
    // If you don't care about the order of the elements inside
    // the array, you should sort both arrays here.
    // Please note that calling sort on an array will modify that array.
    // you might want to clone your array first.
  
    for (var i = 0; i < a.length; ++i) {
      if (a[i] !== b[i]) return false;
    }   
    return true;

}
//snodd kod
function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}


function DraftableCardsFromServer( textArray)
{
    for(i = 0; i < textArray.length;i++)
    {
        CreateDraftableCard(textArray[i],i);
    }
}



function StartDraft()
{

    
    amountOfPacks = inputAmountOfPacks.value;
    cardsPerPack = inputCardsPerPack.value;
    //amountOfPlayers = inputAmountOfPlayers.value;
    
    
    startForm.remove();

  //  ShowDraftableCards();
  // ShowDraftableCardsServer();
   // StartInitialization();

   // ShowPack(simulatedPacks[0]);

}



//Tagen från https://ourcodeworld.com/articles/read/189/how-to-create-a-file-and-generate-a-download-with-javascript-in-the-browser-without-a-server
function download(filename, text) {
    var element = document.createElement('a');
    element.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
    element.setAttribute('download', filename);
  
    element.style.display = 'none';
    document.body.appendChild(element);
  
    element.click();
  
    document.body.removeChild(element);
  }





function RemoveDraftableCards()
{
    while (draftableCardSection.firstChild) {
        draftableCardSection.removeChild(draftableCardSection.lastChild);
      }
}
function RemoveDraftedCards()
{
    while (draftedCardsSection.firstChild) {
        draftedCardsSection.removeChild(draftedCardsSection.lastChild);
      }
}

function RemoveChildren(htmlElement)
{
    while (htmlElement.firstChild) {
        htmlElement.removeChild(htmlElement.lastChild);
      }
}

async function PickCardServer(cardIndex)
{
    response = await fetch("PickCard",{
        method: "Post",
        body: cardIndex
    });
    UpdateDraftedCards();

}



function CreateDraftableCard(cardName, index)
{   
    let card = document.createElement("picture");

    let cardImage = document.createElement("img")

    cardImage.src = "CardImages/" + cardName; 

    card.append(cardImage);

    console.log("CardImages/" + cardName);

    cardImage.className = "DraftableCard";


    card.setAttribute(DATA_INDEX, index);

    card.addEventListener("click", ()=>{

        
        AddCardToDraftPile(cardName);
        
        RemoveDraftableCards();
        HideHighlightCard();
        
        PickCardServer(index);
        
        
    //    console.log(simulatedPacks[0].length);

        
    }, {signal });

    card.addEventListener("pointerenter",()=>{
        ShowHighlightCard(cardName);
    });
    card.addEventListener("pointerleave",HideHighlightCard);


    //console.log("detta hander");

    

    draftableCardSection.append(card);

    return card;
}


function ShowHighlightCard(cardName)
{   

    cardHighlight.src = "CardImages/" + cardName;
    cardHighlight.hidden = false;

}

function HideHighlightCard()
{
   cardHighlight.hidden = true;

}

function AddCardToDraftPile(cardName)
{   

   // event.srcElement.remove();

    //draftedCards = UpdateDraftedCards();

    HideHighlightCard();


    let cardImage = document.createElement("img")

    cardImage.src = "CardImages/" + cardName; 
    cardImage.className = "DraftedCard";

    draftedCardsSection.append(cardImage);

    cardImage.addEventListener("pointerenter",()=>{
       ShowHighlightCard(cardName); 
    });
    cardImage.addEventListener("pointerleave",()=>{
        HideHighlightCard(); 
     });

    //draftedCards.push(cardName);
}

async function UpdateDraftedCards()
{
    response = await fetch("/PickedCards",{
            method: "Get"
            }).then((response) => response.text()).then((text) => {

                RemoveDraftedCards();
                message = text.split(":");
                
                sortedList = ReturnSortedDraftedCards(message);
                for(i = 0; i < sortedList.length; i++)
                {
                    AddCardToDraftPile(sortedList[i]);
                }

            });
}

function ReturnSortedDraftedCards(listToSort)
{
    let mainDeckCards = [];
    let extraDeckCards = [];

    let listToReturn = [];

    for( i = 0; i < listToSort.length;i++)
    {
        if(listToSort[i].includes("Extra"))
        {
            extraDeckCards.push(listToSort[i]);
        }
        else
        {
            mainDeckCards.push(listToSort[i])
        }
    }
 
    for(i = 0; i < mainDeckCards.length; i++)
    {
        listToReturn.push(mainDeckCards[i]);
    }
    for(i = 0; i < extraDeckCards.length; i++)
    {
        listToReturn.push(extraDeckCards[i]);
    }

    return listToReturn;

}

function TrimCardArray(listToSearch, stringToMatch)
{
    let listToReturn = [];
    for(i = 0; i < listToSearch.length; i++)
    {
        console.log("vad searchas")
        console.log(listToSearch[i]);
        if(listToSearch[i].includes(stringToMatch))
        {
            console.log("har hittats")

            removedJPG = listToSearch[i].split(".");
            console.log(removedJPG);
            removedPath = removedJPG[0].split(stringToMatch);
         //   removeComma = removedPath.slice(0,1);
            console.log(removedPath)
            listToReturn.push(removedPath[1]);
        }
    }
    return listToReturn;
}

async function FinishDraftAndShowCards()
{   
    continuePlayerUpdate = false;
    let cardsDrafted = "";
    let draftedCards;
    response = await fetch("/PickedCards",{
            method: "Get"
            }).then((response) => response.text()).then((text) => {

                draftedCards = text.split(":");


            });

    for(i = 0; i < draftedCards.length; i++)
    {    

       CreateDraftableCard(draftedCards[i],i);

       //let name =  draftedCards[i].split(".");
        //console.log(name);
       //cardsDrafted += name[0] + "\n";
       
    }
    cardsDrafted += "#main" + "\n";
    arrayToLoop = TrimCardArray(draftedCards,"MainDeck/");

    for(i = 0; i <arrayToLoop.length ; i++)
    {   
        
        cardsDrafted += arrayToLoop[i] + "\n";
    }
    cardsDrafted += "#extra" + "\n";
    arrayToLoop = TrimCardArray(draftedCards,"ExtraDeck/");
    for(i = 0; i <arrayToLoop.length ; i++)
    {
        cardsDrafted += arrayToLoop[i] + "\n";
    }
    cardsDrafted += "!side" + "\n"

    downloadButton.hidden = false;

    downloadButton.addEventListener("click", ()=>{

        download("Drafted deck",cardsDrafted);
    })
    currentDraftableCards = [];
    controller.abort();
    await fetch("/ReceivedDraftFinished",{
        method: "Post"
    });
}
