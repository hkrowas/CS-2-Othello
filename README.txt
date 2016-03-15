Harrison Krowas: Contributed ideas for runtime acceleration. Improved and fine-tuned heuristic.

Improvements:

The game tree is allocated in the player constructor with a single "new" operation. This cuts down on runtime by eliminating the constant time memory allocation overhead associated with each "new" operation. Because of this  improvement, the AI runs out of memory (750 MB) well before the game clock expires. It is able to compute the game tree to around level 7 or 8 before running out of memory.

Our AI also has built-in iterative deepening. When the program senses it is out of memory, it records to which level it has fully computed the game tree and passes it to the findMinimax function. This allows the AI to get the most out of its memory allocation.

One idea discussed but not implemented was to remove parts of the game tree that were no longer needed, namely nodes between the very children and the very parent. This would reduce memory usage by about 20-30%.