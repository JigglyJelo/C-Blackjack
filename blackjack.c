#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cards.h"

#define COMMAND_BUFFER 11
#define MAX_HAND_SIZE 15

int money = 100;
int deckIndex;
Card* deck;
Card playerHand[MAX_HAND_SIZE];
Card dealerHand[MAX_HAND_SIZE];
int playerHandSize;
int dealerHandSize;
char command[COMMAND_BUFFER];

void clearTerminal(){
    printf("\033[H\033[J");
}

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
    clearTerminal();
    printf("\nDealers Hand: ");
    printHand(dealerHand,dealerHandSize - (showDealersHand ? 0 : 1));
    printf("\nYour Hand: ");
    printHand(playerHand,playerHandSize);
}

enum command{
    ERROR, HIT, STAND, DOUBLE_DOWN, CONTINUE, QUIT
};

enum command getCommand(){
    if(fgets(command, sizeof(command), stdin)) {
        switch(command[0]){
            case 'h': case 'H': return HIT;
            case 's': case 'S': return STAND;
            case 'd': case 'D': return DOUBLE_DOWN;
            case 'c': case 'C': return CONTINUE;
            case 'q': case 'Q': return QUIT;
            default: return ERROR;
        }
    }else{
        return ERROR;
    }
}

void playerTurn(){
    while(1){
        printGameState(false);
        printf("\nHit, Stand, or Double Down?\n");
        switch(getCommand()){
            case HIT:
                playerHand[playerHandSize++] = drawCardFromDeck(&deck,&deckIndex,52);
                break;
            case STAND:
                return;
            case DOUBLE_DOWN:
                playerHand[playerHandSize++] = drawCardFromDeck(&deck,&deckIndex,52);
                return;
            default:
                printf("\nInvalid command");
                break;
        }
        if(getHandValue(playerHand,playerHandSize) > 21) return;
    }
}

void dealerTurn() {
    int playerHandValue = getHandValue(playerHand, playerHandSize);
    // If the player already lost, don't do the dealer's turn
    if (playerHandValue > 21) return;
    printGameState(true);
    printf("Dealer revealed their 2nd card %s of %ss (Press Enter to continue)",rankToString(dealerHand[1].rank),suitToString(dealerHand[1].suit));
    fgets(command, sizeof(command), stdin);  //Wait for user confirmation
    //Otherwise, start the dealer's turn
    while(1){
        printGameState(true);
        int dealerHandValue = getHandValue(dealerHand, dealerHandSize);
        //Check for soft 17s
        bool isSoft17 = false;
        if(dealerHandValue == 17) {
            for (int i = 0; i < dealerHandSize; i++) {
                if (dealerHand[i].rank == ACE) {
                    isSoft17 = true;
                    break;
                }
            }
        }
        //Hit below 17 or on soft 17, otherwise stand
        if((dealerHandValue >= 17 && !isSoft17 && dealerHandValue >= playerHandValue) || dealerHandValue > 21) {
            return;
        }else{
            Card newCard = drawCardFromDeck(&deck, &deckIndex, 52);
            dealerHand[dealerHandSize++] = newCard;
            printGameState(true);
            printf("Dealer drew %s of %ss (Press Enter to continue)",rankToString(newCard.rank),suitToString(newCard.suit));
            fgets(command, sizeof(command), stdin);  ////Wait for user confirmation
        }
    }
}

enum RoundState{
    WON = 1,
    LOST = -1,
    TIED = 0
};

enum RoundState winConditionCheck() {
    int dealerHandValue = getHandValue(dealerHand, dealerHandSize);
    int playerHandValue = getHandValue(playerHand, playerHandSize);

    if(playerHandValue > 21) return LOST; //Player Bust
    if(dealerHandValue > 21) return WON; //Dealer Bust
    if(playerHandValue > dealerHandValue) return WON;
    if(playerHandValue < dealerHandValue) return LOST;
    return TIED;
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
            case WON:
                printf("\nYou Win!");
                break;
            case LOST:
                printf("\nYou Lost...");
                break;
            case TIED:
                printf("\nIts a Draw");
                break;
        }
        printf("\nEnter Q to quit Enter any other Character to Continue: ");
        switch(getCommand()){
            case QUIT: free(deck); return;
        }
    }
}

int main(){
    #ifdef _WIN32
        system("chcp 65001");
        clearTerminal();
    #endif
    gameLoop();
    return 0;
}