# LittleFS Setup

This project now uses LittleFS to store HTML pages separately from the code.

## Files Structure
```
project/
├── data/              # HTML files stored here
│   ├── page0.html    # Main menu page
│   └── page2.html    # Network configuration page
├── src/              # Source code
└── platformio.ini    # Configuration
```

## How to Upload HTML Files

### First Time Setup:
1. Build the project: `pio run`
2. Upload the filesystem: `pio run --target uploadfs`
3. Upload the code: `pio run --target upload`

### Updating HTML Only:
If you only change HTML files in the `data/` folder:
```bash
pio run --target uploadfs
```

### Updating Code Only:
If you only change .ino files:
```bash
pio run --target upload
```

## Editing HTML Pages

### Static Pages (page0.html, page2.html):
- Edit the HTML files directly in the `data/` folder
- Use any HTML editor or IDE
- No need to escape quotes or concatenate strings
- Upload with `pio run --target uploadfs`

### Dynamic Pages (page1.html - Switches):
- This page is generated in code (PgSwitches.ino) because it has dynamic button states
- The HTML template is in the raw string literal in the code
- Edit the raw string in `PgSwitches.ino` if you need to change the layout

## Benefits:
- ✅ Easy HTML editing - just edit plain HTML files
- ✅ Faster development - no recompilation for HTML changes
- ✅ Cleaner code - HTML separated from C++
- ✅ Can update HTML via OTA in the future
