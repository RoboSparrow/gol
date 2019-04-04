- path init -> main root dir:
    - system agnostic way to find dir
- cell parser -> save pattern: add compress (trailing dead cells)
- improve utils functons, add tests
x app state struct
x replace pattern list with generic array (c++ vector like) solution
- genlist functions, return int status insead void func to check success => pattern_load_patternlist()
- pattern and widget allocate/free pointers (char*, SDl structs)

sdl:
- view pattern static (for preview and future editor)
- calculate screen canvas dims and set dynamic size
- zoom (center world on start)
- widgets: error handling
- sdl message overlay (errors, notifications etc)

milestones
 x rle/cell parser
 - load pattern screen
 - load/save game screen
 - editor screen
 - win32 support (fs)
 - wasm
