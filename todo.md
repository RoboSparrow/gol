- path init -> main root dir:
    - system agnostic way to find dir
- cell parser -> save pattern: add compress (trailing dead cells)
- improve utils functons, add tests
x app state struct
x replace pattern list with generic array (c++ vector like) solution
- genlist functions, return int status instead void func to check success => pattern_load_patternlist()
x pattern and widget allocate/free pointers (char*, SDl structs)
- gol_merge_data: guard against src memory > targ
- load pattern list: pagination
- load pattern list: pagsort by mdate (savegame)
- optarg -R game starting with empty world screen

sdl:
x widget blending color
x view pattern static (for preview and future editor)
x pattern preview: add cols/rows info
- calculate screen frame dims and set dynamic size
x zoom
x pan
x center world on screen
- only render units in frame
- widgets: error handling
- sdl message overlay (errors, notifications etc)

milestones
 x rle/cell parser
 - load pattern screen
 - load/save game screen
 - editor screen
 - win32 support (fs)
 - wasm
