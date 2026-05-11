# Built-in Nodes

The NodeEditor includes **~157 built-in nodes** across 12 active categories. Nodes are registered via `registerDefaultNodeTypes()` in `DefaultNodes.cpp`.

## 📂 Node Documentation by Category

Detailed docs with inputs, outputs, and usage examples are available for each category:

| Category | Nodes | Docs |
|---|---|---|
| [Input](nodes/categories/Input/readme) | 1 | JSON file reader |
| [SubGraph](nodes/categories/SubGraph/readme) | 3 | Nested graph containers |
| [System](nodes/categories/System/readme) | 5 | OS, time, clipboard, process |
| [Math](nodes/categories/Math/readme) | 41 | Arithmetic, trig, vectors, random |
| [Color](nodes/categories/Color/readme) | 16 | Create, manipulate, convert, palettes |
| [Data](nodes/categories/Data/readme) | 9 | JSON, serialization, file I/O |
| [Logic](nodes/categories/Logic/readme) | 7 | AND/OR/NOT, compare, check |
| [Events](nodes/categories/Events/readme) | 6 | Begin, tick, timer, signals |
| [Generators](nodes/categories/Generators/readme) | 5 | Sine wave, noise, oscillator, camera |
| [Output](nodes/categories/Output/readme) | 28 | Display (inline), charts, debug, export |
| [Qt](nodes/categories/Qt/readme) | 10 | QObject, widgets, QML bridge |
| [Utility](nodes/categories/Utility/readme) | 26 | Flow, value, collections, strings, hash |
| **Total** | **~157** | |

## Summary by Category

### Math (41 nodes)
**Basic Operations:** add, subtract, multiply, divide, modulo, power, squareRoot, absolute, negate, clamp, remap, lerp, smoothstep, round, floor, ceil, fract, sign, normalize
**Comparison:** equal, notEqual, greaterThan, lessThan, greaterEqual, lessEqual, compare, clamp, min, max, average, sum
**Trigonometry:** sin, cos, tan, asin, acos, atan, degToRad, radToDeg
**Vector:** vec2, vec3, vec4, dotProduct, crossProduct, length, distance
**Random:** randomFloat, randomInteger, noise, seedRandom

### Logic (7 nodes)
**Basic:** and, or, xor, not
**Compare:** booleanCompare
**Check:** isTrue, isFalse

### Utility (26 nodes)
**Flow:** sequence, branch, switch, gate, delay, timer
**Value:** reroute, cache, constant, debugPrint, watch, typeConvert
**Collections:** makeArray, appendArray, removeArrayItem, count, contains
**Strings:** concat, split, replace, regex, toUpper, toLower
**Hash:** uuidGenerate, md5, sha256

### Color (16 nodes)
**Basics:** rgb, hsv, hex, fromString
**Manipulation:** blend, multiply, invert, saturation, brightness, contrast
**Conversion:** rgbToHsv, hsvToRgb, rgbToHex, hexToRgb
**Palette:** gradient, generator

### Data (9 nodes)
**JSON:** parse, get, set, array
**Serialization:** serialize, deserialize
**File I/O:** read, write, csvParse

### Events (6 nodes)
**Core:** begin, tick, timerEvent, inputEvent, signalEvent, propertyChanged

### Generators (5 nodes)
**Wave:** sine, noise, oscillator
**Color:** gradient
**Media:** camera

### Output (28 nodes)
**Value (inline):** numberDisplay, textDisplay, booleanIndicator, vectorDisplay
**Chart (inline):** linePlot, scatterPlot, barChart, pieChart, histogram, heatmap
**Matrix (inline):** matrixDisplay, memoryViewer
**Utility (inline):** progressBar, gauge, compass, fpsMeter
**LED Matrix (inline):** ledMatrixDisplay
**Visual:** imageOutput, videoOutput, viewportOutput, pixelBufferOutput
**Data:** fileWriter, jsonExport
**Debug:** print, logger, graphInspector, watchValue, performanceMonitor

### System (5 nodes)
**Info:** currentTime, envVar, osInfo
**I/O:** clipboard
**Process:** processExecute

### Qt (10 nodes)
**QObject:** connectSignal, emitSignal, setProperty, getProperty, invokeMethod
**Widgets:** buttonClicked, sliderValue, textChanged
**QML:** qmlProperty, qmlSignal

### Input (1 node)
**File:** jsonInput

### SubGraph (3 nodes)
**Interface:** canvasInput, canvasOutput
**Container:** canvasNode

## Planned Categories

| Category | Status |
|---|---|
| Audio | ⏳ Requires Qt Multimedia |
| Device | ⏳ Requires serial/MIDI/GPIO libs |
| Network | ⏳ Requires Qt Network |
| Script | ⏳ Requires scripting backend |
| Video | ⏳ Requires Qt Multimedia/FFmpeg |
