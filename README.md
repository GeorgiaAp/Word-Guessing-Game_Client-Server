# Word Guessing Game (Client-Server)

This is a simple word guessing game where the server sends a randomly chosen word to the client, and the client tries to guess the word by guessing individual letters. The game uses shared memory for communication between the client and server, and message queues for message passing.

## Features
- Client-server architecture
- Server sends a word with the first and last letters revealed, and the rest are replaced by dashes.
- The client guesses one letter at a time.
- The server validates the guess and sends updated word progress back to the client.
- The game continues until the client guesses all letters correctly or runs out of tries.

## Files
- **client.c**: Contains the client-side code.
- **server.c**: Contains the server-side code.
- **dictionary.txt**: Contains the list of words that the server will randomly pick from.
- **README.md**: Documentation for the project.

## Building and Running
1. Compile the client and server:
  - gcc -o client client.c
  - gcc -o server server.c
2. Run the server in one terminal:
  - ./server
3. Run the client in a seperate terminal:
  - ./client

## Gameplay
- The server will ask you for the dictionary file and send a random word.
- The client will start guessing letters one at a time.
- The client receives the word progress and number of tries remaining after each guess.
- The game ends when the client either guesses the entire word or runs out of tries.

