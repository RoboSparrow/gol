- main root dir:
    - system agnostic way to find dir
    - protect root dir var
x ~~rle-parser: better identification of comment lines (snprintf)~~
x rle_load_meta & rle_load_data, option for using same filepointer
x pattern, meta alloc: check null
x pattern offset center option
x pattern struct: ~~add offsets~~, data, ~~type props~~
x rle-parser: save as rle
- cell-parser: http://conwaylife.com/wiki/Plaintext

sdl:
- view pattern static (for preview and future editor)
- zoom (center world on start)

milestones
 - rle/cell parser
 - load/save screen
 - editor screen
 - win32 support (fs)
 - wasm
