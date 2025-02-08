#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef CARDS_H
#define CARDS_H

enum Suit{
    SPADE = 0,
    HEART = 1,
    CLUB = 2,
    DIAMOND = 3
};

enum Rank{
    JOKER = 0,
    ACE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    JACK = 11,
    QUEEN = 12,
    KING = 13,
};

typedef struct{
    enum Suit suit:2;
    enum Rank rank:4;
} Card;

Card createCard(enum Suit suit, enum Rank rank){
    Card card;
    card.suit = suit;
    card.rank = rank;
    return card;
}

// Function to shuffle the deck
void shuffleDeck(Card** deck, int deckSize) {
    Card* deckArr = *deck;  // Dereference the double pointer to get the actual array of cards

    // Seed the random number generator to get different shuffles each time
    srand(time(NULL));

    // Fisher-Yates shuffle algorithm
    for (int i = deckSize - 1; i > 0; i--) {
        // Get a random index between 0 and i
        // Generate a random integer
        int randInt = rand();
        // Calculate the number of bits needed to represent deckSize
        int numBits = (int)ceil(log2(deckSize));
        // Create a mask with the appropriate number of bits set to 1
        int mask = (1 << numBits) - 1;
        // Get a masked random index between 0 and i
        int j = (randInt & mask) % (i + 1);

        // Swap d[i] with d[j]
        Card temp = deckArr[i];
        deckArr[i] = deckArr[j];
        deckArr[j] = temp;
    }
}

Card* create52CardDeck(){
    Card* deck = (Card*)malloc(52 * sizeof(Card));
    if(deck == NULL){
        // NO RAM
        return NULL;
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 13; j++){
            deck[(i * 13) + j] = createCard(i,j+1);
        }
    }
    shuffleDeck(&deck,52);
    return deck;
}

Card* create54CardDeck(){
    Card* deck = (Card*)malloc(54 * sizeof(Card));
    if(deck == NULL){
        // NO RAM
        return NULL;
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 13; j++){
            deck[(i * 13) + j] = createCard(i,j+1);
        }
    }
    deck[53] = createCard(0,JOKER);
    deck[54] = createCard(0,JOKER);
    shuffleDeck(&deck,54);
    return deck;
}

Card drawCardFromDeck(Card** deck, int* deckIndex, int deckSize){
    Card topCard = (*deck)[*deckIndex];
    (*deckIndex)++;
    if(*deckIndex >= deckSize){
        *deckIndex = 0;
        shuffleDeck(deck,deckSize);
    }
    return topCard;
}

const char* suitToString(enum Suit suit) {
    switch(suit){
        case SPADE: return "Spade";
        case HEART: return "Heart";
        case CLUB: return "Club";
        case DIAMOND: return "Diamond";
        default: return "Error";
    }
}

const char* rankToString(enum Rank rank) {
    switch(rank){
        case JOKER: return "Joker";
        case ACE: return "Ace";
        case TWO: return "Two";
        case THREE: return "Three";
        case FOUR: return "Four";
        case FIVE: return "Five";
        case SIX: return "Six";
        case SEVEN: return "Seven";
        case EIGHT: return "Eight";
        case NINE: return "Nine";
        case TEN: return "Ten";
        case JACK: return "Jack";
        case QUEEN: return "Queen";
        case KING: return "King";
        default: return "Undefined";
    }
}

char* cardToString(Card card) {
    // Get the lengths of rank and suit strings
    size_t rankLen = strlen(rankToString(card.rank));
    size_t suitLen = strlen(suitToString(card.suit));

    // Allocate memory for the result string
    // Format: "rank of suit" + space + "of" + null terminator
    char* result = (char*)malloc(rankLen + suitLen + 4 + 1);  // 4 for " of " and null terminator
    
    if (result != NULL) {
        // Format the string
        snprintf(result, rankLen + suitLen + 4 + 1, "%s of %s", rankToString(card.rank), suitToString(card.suit));
    }

    return result;  // Return the resulting string (caller must free this memory)
}

// Convert Suit enum to ASCII suit symbol
const char* suitToAscii(enum Suit suit) {
    switch(suit){
        case SPADE: return "♠";
        case HEART: return "♥";
        case CLUB: return "♣";
        case DIAMOND: return "♦";
        default: return " ";
    }
}

// Convert Rank enum to ASCII rank symbol
const char* rankToAscii(enum Rank rank) {
    switch(rank){
        case ACE: return "A";
        case TWO: return "2";
        case THREE: return "3";
        case FOUR: return "4";
        case FIVE: return "5";
        case SIX: return "6";
        case SEVEN: return "7";
        case EIGHT: return "8";
        case NINE: return "9";
        case TEN: return "10";
        case JACK: return "J";
        case QUEEN: return "Q";
        case KING: return "K";
        case JOKER: return "JOKER";
        default: return " ";
    }
}

// Print ASCII art for a single card
void printCardAsciiArt(Card card) {
    const char* rank = rankToAscii(card.rank);
    const char* suit = suitToAscii(card.suit);
    // Print the top border of the card
    printf("┌─────┐");

    // Print the middle part of the card
    printf(" ");
    if(strlen(rank) == 1){
        printf("|%s    ", rank);
    }else{
        printf("|%s   ", rank);
    }
    printf("|");
    printf("\n");

    // Print the suit part of the card
    printf("|  %s  |", suit);
    printf(" ");
    printf("\n");

    // Print the bottom part of the card
    if(strlen(rank) == 1){
        printf("|    %s|", rank);
    }else{
        printf("|  %s|", rank);
    }
    printf("\n");

    // Print the bottom border of the card
    printf("└─────┘");

    printf(" ");
}



// Print multiple cards side by side
void printHandAscii(Card* hand, int handSize) {
    if (handSize <= 0) return;  // No cards to print

    // Print the top border of all cards
    for (int i = 0; i < handSize; i++) {
        printf("┌─────┐ ");
    }
    printf("\n");

    // Print the middle part of all cards
    for(int i = 0; i < handSize; i++){
        const char* rank = rankToAscii(hand[i].rank);
        if(strlen(rank) == 1){
            printf("│%s    ", rank);
        }else{
            printf("│%s   ", rank);
        }
        printf("│ ");
    }
    printf("\n");

    // Print the suit part of all cards
    for (int i = 0; i < handSize; i++) {
        printf("│  %s  │", suitToAscii(hand[i].suit));
        printf(" ");
    }
    printf("\n");

    // Print the bottom part of all cards
    for(int i = 0; i < handSize; i++){
        // Print the bottom part of the card
        const char* rank = rankToAscii(hand[i].rank);
        if(strlen(rank) == 1){
            printf("│    %s│", rank);
        }else{
            printf("│   %s│", rank);
        }
        printf(" ");
    }
    printf("\n");

    // Print the bottom border of all cards
    for(int i = 0; i < handSize; i++){
        printf("└─────┘ ");
    }
    printf("\n");
}

#endif