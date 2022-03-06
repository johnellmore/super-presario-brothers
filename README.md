# Super Presario Brothers

This is my final project from my freshman CS120 course in college years ago. The project was to make a simple game in C++. I had a lot of fun building this 2D Mario-like platformer.

## Running

This only runs on Windows, since the school-provided arcade.h/arcade.cpp is Windows-only. Someday I might get around to porting it to other platforms for fun. The API is certainly simple enough.

The `mario.exe` file is a precompiled version that you can run directly. It's not a virus, I promise. But still, you shouldn't go around trusting the word of random people on the internet.

## Building

I don't remember the specific build instructions offhand; the main build command is something like `g++ -Wall mario.cpp arcade.cpp -o mario.exe`.

## Making levels

(from my original assignment notes)

> To make your own levels for Super Presario Brothers, open up a text editor like Notepad. Save this file to the "levels" subfolder as level{N}.lvl, where {N} is the first number not already taken.
>
> Now you can start making your level. I'd recommend taking a look at a pre-existing level file to get an idea of how they work. "level1.lvl" is a good example.
>
> Level files are laid out in a grid, with exactly 14 rows and an arbitrary number of columns. Trailing spaces at the end of each row can be optionally omitted, but your level will be as long as your longest row is.
>
> The following symbols can be used:
>
> P
> This represents the player's inital location. There should only be one of these, but if there are multiple ones, the last one in the file will be used.
>
> S
> These are stars that the player must pick up in order to win the level
>
> {space}
> These represents air. The player can pass through this.
>
> G
> These are ground tiles. They should be used for the player to walk on. They should always be at the bottom of the screen, and never floating in the air.
>
> B
> These are floating platforms for the user to walk on. They should only be used in the air, and not on the ground.
>
> C
> These are blocks that sit stacked on the ground. Generally speaking, they shouldn't be floating, but they can be stacked as high as necessary.
>
> J
> This is a spring which launches the player higher than they could normally jump.
>
> M
> These are mines. They can be placed anywhere. The player will die if they make contact with these.
>
> X
> These are invisible death traps. They will kill the player on contact. Their primary purpose is to line the bottom of pits so that the player will not keep falling off the screen, but they are optional as the player will die anyway if they fall off the bottom.