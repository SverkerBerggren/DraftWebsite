

let draftableCardSection = document.getElementById("draftableCardsSection");

let draftedCardsSection = document.getElementById("DraftedCardContainer");

let cardHighlight = document.getElementById("HighLightCard");

let downloadButton = document.getElementById("Downloadbutton");

let hostButton = document.getElementById("HostButton");
let startButton = document.getElementById("StartButton");

let startForm = document.getElementById("StartForm");

let joinButton = document.getElementById("JoinButton");
let joinInput = document.getElementById("InputFieldJoin");


let inputCardsPerPack = document.getElementById("InputFieldCardPerPack");
let inputAmountOfPacks = document.getElementById("InputFieldAmountOfPack");
let inputAmountOfPlayers = document.getElementById("InputFieldAmountOfPlayers");


TestaFunktioner();

HideHighlightCard();

joinButton.onclick = JoinLobby;


const DATA_INDEX = "data-index";


const controller = new AbortController();
const { signal } = controller;


// const draftedCards = [];

let continueUpdate = true;
//AddAvailableCards();

let currentDraftableCards = [];
hostButton.onclick = HostLobby;


//ShowPack(simulatedPacks[0]);

//HostLobby();

TestGetMethod();

downloadButton.hidden = true;

async function TestGetMethod()
{   

    response2  = await fetch("http://localhost:1234/hi", {
        method: "GET", // *GET, POST, PUT, DELETE, etc.
        headers: {
          "Content-Type": "text/plain",
          // 'Content-Type': 'application/x-www-form-urlencoded',
        },
      });
    
    console.log(response2.text());

}


async function HostLobby()
{

    startForm.remove();
    response = await fetch("http://localhost:1234/HostLobby",{
        method: "Post"
    }).then((response) => response.text()).then((text) =>{
        console.log(text);
    });
    
    //UpdateLoop();
}

async function UpdateLoop()
{
    while(continueUpdate)
    {
        response = await fetch("http://localhost:1234/Update",{
            method: "Get",
            signal: AbortSignal.timeout(15000)
            }).then((response) => response.text()).then((text) => {
                
                message = text.split(":");
                if(message[0] == "DraftableCards")
                {   
                    message.shift();

                    console.log(message);
                    console.log(currentDraftableCards);
                    console.log(arraysEqual(message,currentDraftableCards));
                    if((!arraysEqual(message,currentDraftableCards)) || currentDraftableCards.length == 0)
                    {
                        DraftableCardsFromServer(message);
                    }
                    currentDraftableCards = message;
                }
                if(message[0] == "DraftFinished")
                {
                    FinishDraftAndShowCards();
                    continueUpdate = false;
                }
                console.log(text);
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
async function JoinLobby()
{
    response = await fetch("http://localhost:1234/JoinLobby",{
        method: "Post",
        body: joinInput.value
        }).then((response) => response.text()).then((text) => {
            if(text == "Accepted")
            {
                startForm.remove();
            }

            console.log(text);
          });
      
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
    amountOfPlayers = inputAmountOfPlayers.value;
    
    
    startForm.remove();

  //  ShowDraftableCards();
  // ShowDraftableCardsServer();
   // StartInitialization();

   // ShowPack(simulatedPacks[0]);

}


async function ShowDraftableCardsServer()
{
    await fetch("http://localhost:1234/AvailableCards",{
    method: "Get"
    }).then((response) => response.text()).then((text) => {
        console.log(text);
        cards = text.split(":");
        console.log(cards);

        for(i = 0; i  < cards.length; i++)
        {
            CreateDraftableCard(cards[i],i);
        }
      });
  
    

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


    console.log("detta hander");

    

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
    response = await fetch("http://localhost:1234/PickedCards",{
            method: "Get"
            }).then((response) => response.text()).then((text) => {

                RemoveDraftedCards();
                message = text.split(":");
                for(i = 0; i < message.length; i++)
                {
                    AddCardToDraftPile(message[i]);
                }

            });
}

async function FinishDraftAndShowCards()
{   

    let cardsDrafted = "";
    let draftedCards;
    response = await fetch("http://localhost:1234/PickedCards",{
            method: "Get"
            }).then((response) => response.text()).then((text) => {

                draftedCards = text.split(":");


            });

    for(i = 0; i < draftedCards.length; i++)
    {    

       CreateDraftableCard(draftedCards[i],i);

       
       let name =  draftedCards[i].split(".");
        console.log(name);
       cardsDrafted += name[0] + "\n";
       
    }

    downloadButton.hidden = false;

    downloadButton.addEventListener("click", ()=>{

        download("Drafted deck",cardsDrafted);
    })

    controller.abort();
}

function TestaFunktioner()
{   

    $(document).on("keypress", function (e) {
        // use e.which
        
      //  CreateDraftableCard("Ash.webp");
      
    });
}