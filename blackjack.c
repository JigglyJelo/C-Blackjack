#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cards.h"

#define COMMAND_BUFFER 11
#define MAX_HAND_SIZE 15
#define STARTING_MONEY 100
#define DEFAULT_BET 10

int money = 100;
int bet = 0;
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
    printf("MONEY: $%d",money);
    if(bet != 0) printf(" Current Bet: $%d",bet);
    printf("\n\nDealers Hand: ");
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
                if(money >= bet){
                    money -= bet;
                    bet *= 2;
                    playerHand[playerHandSize++] = drawCardFromDeck(&deck,&deckIndex,52);
                    return;
                }else{
                    printf("\nNot enough money to double down.");
                    break;
                }
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
        //Get player bet
        if(money <= 0){
            printGameState(true);
            free(deck);
            return;
        }
        while(1){
            printf("Enter a bet (Blank for default bet $%d) or Q to Quit: ",DEFAULT_BET);
            fgets(command, sizeof(command), stdin);

            // Check if the user just pressed enter
            if(command[0] == '\n'){
                bet = DEFAULT_BET;
                break;
            }else if(command[0] == 'q' || command[0] == 'Q'){
                free(deck);
                return;
            }

            //Try to convert input to an integer
            if(sscanf(command, "%d", &bet) == 1) {
                if(bet >= 1 && bet <= money){
                    break;
                }else{
                    printf("Bet must be between $%d and $%d.\n", 1, money);
                }
            }else{
                printf("Invalid input. Please enter a valid integer.\n");
            }
        }
        money -= bet;
    
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
        switch(winConditionCheck()){
            case WON:
                int profit = bet;
                money += bet*2;
                bet = 0;
                printGameState(true);
                printf("\nYou Win! You profited $%d!\n",profit);
                break;
            case LOST:
                int burntMoney = bet;
                bet = 0;
                printGameState(true);
                printf("\nYou Lost... $%d.\n",burntMoney);
                break;
            case TIED:
                int recoup = bet;
                money += bet;
                bet = 0;
                printGameState(true);
                printf("\nIts a Draw you recouped your $%d bet.\n",recoup);
                break;
        }
    }
}

int main(){
    #ifdef _WIN32
        system("chcp 65001");
        clearTerminal();
    #endif
    gameLoop();
    if(money > 0){
        printf("You ended with $%d",money);
    }else{
        printf("You lost all your money...");
    }
    fgets(command, sizeof(command), stdin);
    return 0;
}