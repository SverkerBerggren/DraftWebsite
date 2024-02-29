
let draftableCardSection = document.getElementById("draftableCardsSection");

let draftedCardsSection = document.getElementById("DraftedCardContainer");

let cardHighlight = document.getElementById("HighLightCard");

let downloadButton = document.getElementById("Downloadbutton");

let startButton = document.getElementById("StartButton");

let startForm = document.getElementById("StartForm");

let inputCardsPerPack = document.getElementById("InputFieldCardPerPack");
let inputAmountOfPacks = document.getElementById("InputFieldAmountOfPack");
let inputAmountOfPlayers = document.getElementById("InputFieldAmountOfPlayers");

const availableCards = []; 

const draftedCards = [];

TestaFunktioner();

HideHighlightCard();


const simulatedPacks = [];
const DATA_INDEX = "data-index";

let amountOfPacks = 3;
let amountOfPlayers = 5;
let cardsPerPack = 10;  

const controller = new AbortController();
const { signal } = controller;


let packsDrafted = 0; 


AddAvailableCards();

startButton.onclick = StartDraft;

//ShowPack(simulatedPacks[0]);


downloadButton.hidden = true;

function StartDraft()
{

    
    amountOfPacks = inputAmountOfPacks.value;
    cardsPerPack = inputCardsPerPack.value;
    amountOfPlayers = inputAmountOfPlayers.value;
    
    
    startForm.remove();
    StartInitialization();

    ShowPack(simulatedPacks[0]);

}



function StartInitialization()
{   



    for(i = 0; i < amountOfPlayers; i++)
    {
        simulatedPacks.push(CreatePack());
    }

    //console.log(simulatedPacks);
}

function GetRandomInt(max )
{
    return Math.floor(Math.random() * max);
}


function CreatePack()
{   


    const cardPack = [];
    for(z = 0; z < cardsPerPack; z++)
    {   
        let index = GetRandomInt(availableCards.length);
        cardPack.push(availableCards[index]);
    }

    
    

    return cardPack ;
}

function OpenNewPack()
{

    simulatedPacks.length = 0;


    for(i = 0; i < amountOfPlayers; i++)
    {
        simulatedPacks.push(CreatePack());
    }


}



function RotatePacks()
{
    
    let packToPlace;

    for(i = 0; i< simulatedPacks.length; i++)
    {

        if(i == 0)
        {
            packToPlace = simulatedPacks[1];
            simulatedPacks[1] = simulatedPacks[0];
            continue;
        } 
        if(i == simulatedPacks.length -1)
        {
            simulatedPacks[0] = packToPlace;
            continue;
        }
        let packReplaced = simulatedPacks[i];

        simulatedPacks[i] = packToPlace;
        
        packToPlace = packReplaced;
        



    }


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

function RemoveCardFromPacks(index)
{   

    simulatedPacks[0].splice(index,1);
    

    
    for(i = 1; i < simulatedPacks.length; i++)
    {   
        console.log(simulatedPacks[0]);

        simulatedPacks[i].splice(GetRandomInt(simulatedPacks[i].length),1);
    }

    if(simulatedPacks[0].length == 0)
    {   
        packsDrafted +=1;
        if(packsDrafted < amountOfPacks)
        {

            OpenNewPack();
        }else
        {
            FinishDraftAndShowCards();     
        }
    }
}


function ShowPack(packToShow)
{   
    if(packToShow.length == 0)
    {
        return;
    }

    for(i = 0; i < packToShow.length; i++)
    {
        CreateDraftableCard(packToShow[i],i);
    }

}

function RemoveDraftableCards()
{
    while (draftableCardSection.firstChild) {
        draftableCardSection.removeChild(draftableCardSection.lastChild);
      }
}


function AddAvailableCards()
{
    availableCards.push("Ash.webp");
    availableCards.push("InstantFusion.png");
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

        RemoveCardFromPacks(index);

        RotatePacks();

   

        if(simulatedPacks[0].length != 0)
        {

            ShowPack(simulatedPacks[0]);    
        }

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