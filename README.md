# wordle.c

Wordle game written in C in less than 200 loc.

To compile:
```zsh
gcc wordle.c -o wordle
```

To run:
```zsh
./wordle
```

### Advanced options
You can customize the game difficulty using flags:
- `-s [number]`: Set the lenght of the word to guess.
- `-a [number]`: Set the maximum Attempts.
- `-it`: Set dictionary to italian.

By default, wordle will run in with 5 characters long english words.
