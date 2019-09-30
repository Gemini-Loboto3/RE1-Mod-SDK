# FILE Editor
A very simple FILE editor to generate images used in documents.

## How to use
Just generate a new file, save it, export it whenever you're done.

If you have too many files that you want to export at once, use the Batch Converted under Tools. Be careful, tho, since it will use the base XML name to generate sequences of PNGs (i.e. `FILEM_Q.xml` will generate `FILEM_Q00.png`, `FILEM_Q01.png`, etc).

If you need any extra glyphs, edit `font.png` and `encoding.xml` to give yourself new characters. `font.png` is assembled in a 16x16 grid, while `encoding.xml` is exactly the same format as the one used by the Classic REbirth DLL (indent is ignored, tho).

## Special formatting commands
* `{list}`: tabs text for lists.
* `{center}`: aligns text to the horizontal center. Use this for FILE titles in the first page.
* `{right}`: aligns text to the right. Useful for dates or names in signatures.

## How to generate the MOLE password graphics
You pretty much have to copy that from an already existing graphics, since that's also what CAPCOM did on Deadly Silence, where all FILEs are actually text and not images.
