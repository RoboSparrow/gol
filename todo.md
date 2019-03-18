- main root dir:
    - system agnostic way to find dir
    - protect root dir var
- rle-parser: better identification of comment lines (snprintf)
- rle_load_meta & rle_load_data, option for using same filepointer
x pattern, meta alloc: check null
- pattern offset center option
- pattern struct: add offsets, data, type props
- rle-parser: save as rle
- cell-parser: http://conwaylife.com/wiki/Plaintext
 - complete rle parser: reverse- save game as rle

sdl:
- view pattern static (for preview and future editor)
- zoom (center world on start)

milestones
 - rle/cell parser
 - load/save screen
 - editor screen
 - win32 support (fs)
 - wasm
