#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cards.h"

#define COMMAND_BUFFER 11
#define MAX_HAND_SIZE 15

int deckIndex;
Card* deck;
Card playerHand[MAX_HAND_SIZE];
Card dealerHand[MAX_HAND_SIZE];
int playerHandSize;
int dealerHandSize;
char command[COMMAND_BUFFER];

int getHandValue(Card* hand, int handSize){
    int handValue = 0;
    int aceCount = 0;
    for(int i = 0; i < handSize; i++){
        switch(hand[i].rank){
            case JACK:
            case QUEEN:
            case KING:
                handValue += 10;
                break;
            case ACE:
                aceCount++;
                break;
            default:
                handValue += hand[i].rank;
                break;
        }
    }
    //Add aces
    if(aceCount > 0){
        //Start assuming all aces are 1
        handValue += aceCount;
        //Try to replace the 1s with 11s (Adding 10 instead 11 cause we already added 1 above)
        while(aceCount > 0 && handValue + 10 <= 21){
            aceCount--;
            handValue += 10;
        }
    }
    return handValue;
}

void printHand(Card* hand, int handSize) {
    printf("\n");
    printHandAscii(hand, handSize);
    printf("Value: %d\n", getHandValue(hand, handSize));
}

void printGameState(bool showDealersHand){
    printf("\nDealers Hand: ");
    printHand(dealerHand,dealerHandSize - (showDealersHand ? 0 : 1));
    printf("\nYour Hand: ");
    printHand(playerHand,playerHandSize);
}

void playerTurn(){
    while(1){
        printGameState(false);
        printf("\nEnter a command: ");
        if(fgets(command, sizeof(command), stdin)) {
            // Remove newline character if present
            size_t len = strlen(command);
            if (len > 0 && command[len - 1] == '\n') {
                    command[len - 1] = '\0';
                }
                printf("You entered: %s\n", command);
            }else{
            printf("Error reading input.\n");
        }
        switch(command[0]){
            case 'h':
            case 'H': //Hit
                printf("\nYou hit");
                playerHand[playerHandSize++] = drawCardFromDeck(&deck,&deckIndex,52);
                break;
            case 's':
            case 'S': //Stand
                printf("\nYou stand");
                return;
            case 'd':
            case 'D': //Double Down
                printf("\nYou Double Down");
                playerHand[playerHandSize++] = drawCardFromDeck(&deck,&deckIndex,52);
                return;
            default:
                printf("\nInvalid command");
                break;
        }
        if(getHandValue(playerHand,playerHandSize) > 21) return;
    }
}

void dealerTurn(){
    int playerHandValue = getHandValue(playerHand,playerHandSize);
    //If player already lost don't do turn
    if(playerHandValue > 21) return;
    //Else do turn
    while(1){
        printGameState(true);
        int dealerHandValue = getHandValue(dealerHand,dealerHandSize);
        //Check for soft 17s
        bool isSoft17 = false;
        if(dealerHandValue == 17){
            for(int i = 0; i < dealerHandSize; i++){
                if(dealerHand[i].rank == ACE){
                    isSoft17 = true;
                    break;
                }
            }
        }
        //Hit below 17 or on soft 17 otherwise stand
        if((dealerHandValue >= 17 && !isSoft17 && dealerHandValue >= playerHandValue) || dealerHandValue > 21){
            return;
        }else{
            dealerHand[dealerHandSize++] = drawCardFromDeck(&deck,&deckIndex,52);
        }
    }
}

enum RoundState{
    Won = 1,
    Lost = -1,
    Tied = 0
};

enum RoundState winConditionCheck() {
    int dealerHandValue = getHandValue(dealerHand, dealerHandSize);
    int playerHandValue = getHandValue(playerHand, playerHandSize);

    if(playerHandValue > 21) return Lost; //Player Bust
    if(dealerHandValue > 21) return Won; //Dealer Bust
    if(playerHandValue > dealerHandValue) return Won;
    if(playerHandValue < dealerHandValue) return Lost;
    return Tied;
}

void gameLoop(){
    //Starting variables
    int round = 0;
    deckIndex = 0;
    deck = create52CardDeck();
    while(1){
        enum RoundState state;
        playerHandSize = 2;
        dealerHandSize = 2;
        playerHand[0] = drawCardFromDeck(&deck,&deckIndex,52);
        playerHand[1] = drawCardFromDeck(&deck,&deckIndex,52);
        dealerHand[0] = drawCardFromDeck(&deck,&deckIndex,52);
        dealerHand[1] = drawCardFromDeck(&deck,&deckIndex,52);
        printf("\nRound %d", ++round);
        //Do turns
        playerTurn();
        dealerTurn();
        //Print results
        printGameState(true);
        switch(winConditionCheck()){
            case Won:
                printf("\nYou win!");
                break;
            case Lost:
                printf("\nYou Lost :c");
                break;
            case Tied:
                printf("\nDraw");
                break;
        }
    }
    
    free(deck);
}

int main(){
    gameLoop();
    return 0;
}