

let draftableCardSection = document.getElementById("draftableCardsSection");

let draftedCardsSection = document.getElementById("DraftedCardContainer");

let cardHighlight = document.getElementById("HighLightCard");

let downloadButton = document.getElementById("Downloadbutton");

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


const draftedCards = [];


//AddAvailableCards();

startButton.onclick = StartDraft;

//ShowPack(simulatedPacks[0]);

HostLobby();

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
    response = await fetch("http://localhost:1234/HostLobby",{
        method: "Post"
    }).then((response) => response.text()).then((text) =>{
        console.log(text);
    });

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


function StartDraft()
{

    
    amountOfPacks = inputAmountOfPacks.value;
    cardsPerPack = inputCardsPerPack.value;
    amountOfPlayers = inputAmountOfPlayers.value;
    
    
    startForm.remove();

  //  ShowDraftableCards();
  ShowDraftableCardsServer();
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


async function PickCardServer(cardIndex)
{
    response = fetch("PickCard",{
        method: "Post",
        body: cardIndex
    });
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

    event.srcElement.remove();


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

    draftedCards.push(cardName);
}

function FinishDraftAndShowCards()
{   

    let cardsDrafted = "";
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