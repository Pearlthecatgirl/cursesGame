# cursesGame

A Curses Game Engine made to satisfy my need for a diablo clone. I've always wanted to play diablo clones in the terminal, and this is how i plan to do it. 
List of planned features:
<ol>
  Random levels: There will be some handcrafted maps, but a lot of maps will be generated every session to follow the traditional roguelike feel
  Randomised Loot: I wanted to do this for the diablo loot system (and borderlands really)
  Respawning enemies/bosses/uniques: Always hated how D2 bosses don't respawn for the most part, and that you'd have to create an online session for it
  Dedicated Loot Farming: Borderlands players love this one
  Easy modding: Map and Data creation tools
  Scripting language: For modding and other functions (this will unlikely be implemented)
  Story: I'm not an english major. I can't write good stories. People will grill me over it. If i can, I will
</ol>

<body> 
In terms of the engine, I would like to follow in the step of FLARE engine, another FOSS ARPG engine

</body>

Build:
Requires <curses.h> and <unistd.h>. The rest are c standard libraries. Now Requires cmake as sort of a build system. 

```
git clone https://github.com/Pearlthecatgirl/cursesGame.git
cd cursesGame
mkdir build
cd build
cmake ../
make
cp ../data ./
```

