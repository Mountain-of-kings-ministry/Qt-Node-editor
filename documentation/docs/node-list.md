# Built-in Nodes

The NodeEditor includes **153+ built-in nodes** across 15 categories. Nodes are registered via `registerDefaultNodeTypes()`.

## Math (37 nodes)

### Basic Arithmetic
`add`, `subtract`, `multiply`, `divide`, `modulo`, `power`, `sqrt`, `abs`, `negate`, `clamp`, `lerp`, `remap`

### Comparison
`greater`, `less`, `equal`, `notEqual`, `greaterEqual`, `lessEqual`, `compare`

### Advanced
`floor`, `ceil`, `round`, `min`, `max`, `average`, `sum`, `product`, `mapRange`, `smoothstep`

### Trigonometry
`sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `degToRad`, `radToDeg`

### Vector
`vec2`, `vec3`, `vec4`, `vectorAdd`, `vectorScale`, `vectorDot`, `vectorCross`, `vectorLength`, `vectorNormalize`

### Random
`randomFloat`, `randomInt`, `randomSeed`, `randomVector`, `noise`, `randomRange`

## Logic (7 nodes)
`and`, `or`, `not`, `xor`, `if`, `switch`, `select`

## Utility (26 nodes)

### Flow Control
`delay`, `toggle`, `flipFlop`, `gate`, `pulse`, `counter`, `timer`, `sequence`

### Value
`constant`, `variable`, `int`, `float`, `string`, `boolean`, `color`, `array`, `map`, `range`

### Collections
`listGet`, `listSet`, `listLength`, `listAppend`, `listRemove`, `mapGet`, `mapSet`

### String
`stringConcat`, `stringSplit`, `stringLength`, `stringSubstring`, `stringReplace`, `stringToNumber`, `numberToString`

### Hash
`hashMD5`, `hashSHA1`, `hashSHA256`

## Color (16 nodes)

### Basics
`colorCreate`, `colorRGBA`, `colorHSLA`, `colorHex`, `colorLerp`, `colorBrighten`, `colorDarken`

### Manipulation
`colorSaturate`, `colorDesaturate`, `colorInvert`, `colorMix`, `colorBlend`, `colorComplement`

### Conversion
`colorToRGB`, `colorToHSL`, `colorToHex`, `colorToVector`, `vectorToColor`, `colorTemperature`

### Palette
`paletteAnalogous`, `paletteMonochromatic`, `paletteTriadic`, `paletteComplementary`, `paletteSplitComplementary`

## Events (6 nodes)
`onStart`, `onUpdate`, `onKeyPress`, `onMouseClick`, `onTimer`, `onChange`

## Data (9 nodes)

### JSON
`jsonParse`, `jsonStringify`, `jsonGet`, `jsonSet`, `jsonArray`, `jsonObject`

### Serialization
`dataToJSON`, `dataFromJSON`, `dataToBinary`, `dataFromBinary`

### File I/O
`fileRead`, `fileWrite`, `fileExists`, `fileDelete`, `pathJoin`

## Output (28 nodes)

### Visual
`imageOutput`, `videoOutput`, `viewportOutput`, `pixelBufferOutput`

### Value (inline preview)
`numberDisplay`, `textDisplay`, `booleanIndicator`, `vectorDisplay`

### Chart (inline preview)
`linePlot`, `scatterPlot`, `barChart`, `pieChart`, `histogram`, `heatmap`

### Matrix (inline preview)
`matrixDisplay`, `memoryViewer`

### Utility (inline preview)
`progressBar`, `gauge`, `compass`, `fpsMeter`

### LED Matrix (inline preview)
`ledMatrixDisplay`

### Data
`fileWriter`, `jsonExport`

### Debug
`print`, `logger`, `graphInspector`, `watchValue`, `performanceMonitor`

**Note**: Display nodes with "inline preview" render their output visually inside the node body on the canvas, rather than showing raw values in the Properties panel.

## System (5 nodes)
`currentTime`, `envVar`, `osInfo`, `clipboard`, `processExecute`

## Generators (6 nodes)
`sineWave`, `noiseGenerator`, `gradient`, `oscillator`, `cameraFeed`, `inputNode`

## Qt (10 nodes)
`connectSignal`, `emitSignal`, `setProperty`, `getProperty`, `invokeMethod`, `buttonClicked`, `sliderValue`, `textChanged`, `qmlProperty`, `qmlSignal`

## Audio (stub)
Audio nodes are planned but not yet implemented.

## Video (stub)
Video nodes are planned but not yet implemented.

## Network (stub)
Network nodes are planned but not yet implemented.

## Script (stub)
Script nodes are planned but not yet implemented.

## Device (stub)
Device nodes are planned but not yet implemented.
