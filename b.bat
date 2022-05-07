
@echo off


mkdir build
mkdir build2
mkdir build3

echo Building makefont...
mos-a800xl-clang -Oz -Iheader -Isrc -o makefont.xex src\makefont.c src\atari.c src\cio.s

echo rl2.xex - dungeon roguelike...
mos-a800xl-clang  -Oz -Iheader -Isrc -Wno-incompatible-pointer-types-discards-qualifiers -fmerge-all-constants -Wl,--strip-all,--icf=all,-Map=rl2.map,--cref -o rl2.xex src\rl2.c src\die.c src\screen.c src\item.c src\player.c src\map.c src\input.c src\cchar.c src\atari.c src\cio.s src\inventory.c src\utils.c src\ai.c src\dungeon.c src\log.c src\saveload.c src\vbi.s

echo rl3.xex - cavern roguelike...
mos-a800xl-clang  -Oz -Iheader -Isrc -DCAN_DIG  -Wno-incompatible-pointer-types-discards-qualifiers -fmerge-all-constants -Wl,--strip-all,--icf=all,-Map=rl3.map,--cref -o rl3.xex src\rl2.c src\die.c src\screen.c src\item.c src\player.c src\map.c src\input.c src\cchar.c src\atari.c src\cio.s src\inventory.c src\utils.c src\ai.c src\dungeon2.c src\log.c src\saveload.c src\vbi.s

echo rl4.xex - realtime zombie killing roguelike...
mos-a800xl-clang  -Oz -Iheader -Isrc -Wno-incompatible-pointer-types-discards-qualifiers -fmerge-all-constants -Wl,--strip-all,--icf=all,-Map=rl4.map,--cref -o rl4.xex src\rl4.c src\die.c src\screen.c src\item4.c src\player.c src\map.c src\input.c src\cchar4.c src\atari.c src\cio.s src\inventory.c src\utils4.c src\ai.c src\dungeon4.c src\log.c src\saveload.c src\vbi.s 

echo Building three loaders...
\Mad-Assembler-2.1.3\mads -d:rl2 src\ramrom.asm -o:ramrom2.xex
\Mad-Assembler-2.1.3\mads -d:rl3 src\ramrom.asm -o:ramrom3.xex
\Mad-Assembler-2.1.3\mads -d:rl4 src\ramrom.asm -o:ramrom4.xex

echo Building ATRs...

copy data\FONT.DAT build\FONT.DAT
copy data\FONT.DAT build2\FONT.DAT
copy data\FONT.DAT build3\FONT.DAT

copy rl2.xex build\RL2.XEX
copy makefont.xex build\MAKEFONT.XEX
copy ramrom2.xex build\AUTORUN.SYS

copy rl3.xex build2\RL3.XEX
copy makefont.xex build\MAKEFONT.XEX
copy ramrom3.xex build2\AUTORUN.SYS

copy rl4.xex build3\RL4.XEX
copy makefont.xex build\MAKEFONT.XEX
copy ramrom4.xex build3\AUTORUN.SYS

copy story\*.DAT build\
copy story\*.DAT build2\
copy story\*.DAT build3\

copy bin\DOS.SYS build\DOS.SYS
copy bin\DOS.SYS build2\DOS.SYS
copy bin\DOS.SYS build3\DOS.SYS

bin\dir2atr -S -b Dos25 RL2.ATR build
bin\dir2atr -S -b Dos25 RL3.ATR build2
bin\dir2atr -S -b Dos25 RL4.ATR build3

dir rl?.xex

