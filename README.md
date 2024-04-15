A Hangman Game Implementation Using Message Queues, Shared Memory, and Signals. 
The project requires the creation of two different programs, one as a server and one as a client.

Basic functions:
Reading a dictionary from a text file and saving it to a 2D array of characters.
Communication via message queue for the initial connection.
Use shared memory to synchronize and share data.
Communication between server and client through signals.

Game:
The server creates a message queue, allocates shared memory and connects to it.
The client connects to the message queue and shared memory.
The client starts the game with "hi" and receives the word information from the server.
The server informs the client about each step of the game through signals and shared memory.
The process is repeated until the word is found or the attempts are exhausted.
