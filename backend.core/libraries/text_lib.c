//
// Copyright 2016 Timo Kloss
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include "text_lib.h"
#include "core.h"
#include <string.h>
#include <assert.h>

struct Plane *txtlib_getBackground(struct TextLib *lib, int bg)
{
	switch (bg)
	{
	case 0:
		return &lib->core->machine->videoRam.planeA;

	case 1:
		return &lib->core->machine->videoRam.planeB;

	case 2:
		return &lib->core->machine->videoRam.planeC;

	case 3:
		return &lib->core->machine->videoRam.planeD;

	case OVERLAY_BG:
		return &lib->core->overlay->plane;

	default:
		assert(0);
		return NULL;
	}
}

void txtlib_setCellAt(struct Plane *plane, int x, int y, int character, union CharacterAttributes attr)
{
	struct Cell *cell = &plane->cells[y & ROWS_MASK][x & COLS_MASK];
	if (character >= 0)
	{
		cell->character = character;
	}
	cell->attr = attr;
}

void txtlib_scrollRow(struct Plane *plane, int fromX, int toX, int y, int deltaX, int deltaY)
{
	if (deltaX > 0)
	{
		for (int x = toX; x > fromX; x--)
		{
			plane->cells[y][x] = plane->cells[(y - deltaY) & ROWS_MASK][(x - deltaX) & COLS_MASK];
		}
	}
	else if (deltaX < 0)
	{
		for (int x = fromX; x < toX; x++)
		{
			plane->cells[y][x] = plane->cells[(y - deltaY) & ROWS_MASK][(x - deltaX) & COLS_MASK];
		}
	}
	else
	{
		for (int x = fromX; x <= toX; x++)
		{
			plane->cells[y][x] = plane->cells[(y - deltaY) & ROWS_MASK][(x - deltaX) & COLS_MASK];
		}
	}
}

void txtlib_scroll(struct Plane *plane, int fromX, int fromY, int toX, int toY, int deltaX, int deltaY)
{
	if (deltaY > 0)
	{
		for (int y = toY; y > fromY; y--)
		{
			txtlib_scrollRow(plane, fromX, toX, y, deltaX, deltaY);
		}
	}
	else if (deltaY < 0)
	{
		for (int y = fromY; y < toY; y++)
		{
			txtlib_scrollRow(plane, fromX, toX, y, deltaX, deltaY);
		}
	}
	else
	{
		for (int y = fromY; y <= toY; y++)
		{
			txtlib_scrollRow(plane, fromX, toX, y, deltaX, deltaY);
		}
	}
}

void txtlib_scrollWindowIfNeeded(struct TextLib *lib)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->windowBg);

	if (lib->cursorY >= lib->windowHeight)
	{
		// scroll
		txtlib_scroll(plane, lib->windowX, lib->windowY, lib->windowX + lib->windowWidth - 1, lib->windowY + lib->windowHeight - 1, 0, -1);

		// clear bottom line
		int py = lib->windowY + lib->windowHeight - 1;
		for (int x = 0; x < lib->windowWidth; x++)
		{
			int px = x + lib->windowX;
			txtlib_setCellAt(plane, px, py, lib->fontCharOffset, lib->charAttr); // space
		}

		lib->cursorY = lib->windowHeight - 1;

		struct Interpreter *interpreter = lib->core->interpreter;
		if (interpreter->state == StateEvaluate && lib->windowBg != OVERLAY_BG)
		{
			interpreter->waitCount = 1;
			interpreter->exitEvaluation = true;
			interpreter->cycles += lib->windowWidth * lib->windowHeight * 2;
		}
	}
}

void txtlib_printText(struct TextLib *lib, const char *text)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->windowBg);
	const char *letter = text;
	size_t word_len;
	bool auto_newline=false;
count_word_len:
	word_len=0;
	while(letter[word_len] && letter[word_len] != ' ' && letter[word_len] != '\n') word_len++;
	if(word_len>0 && lib->cursorX>0 && lib->cursorX+word_len>lib->windowWidth)// && !auto_newline)
	{
		lib->cursorX=0;
		lib->cursorY++;
	}
	while (*letter)
	{
		if(*letter==' ' && lib->cursorX==0 && auto_newline)
		{
			letter++;
			auto_newline=false;
			goto count_word_len;
		}

		txtlib_scrollWindowIfNeeded(lib);

		if (*letter >= 32)
		{
			char printableLetter = *letter;
			if (printableLetter >= 'a' && printableLetter <= 'z')
			{
				printableLetter -= 32;
			}
			txtlib_setCellAt(plane, lib->cursorX + lib->windowX, lib->cursorY + lib->windowY, lib->fontCharOffset + (printableLetter - 32), lib->charAttr);
			if (lib->windowBg != OVERLAY_BG)
			{
				lib->core->interpreter->cycles += 2;
			}
			lib->cursorX++;
		}
		else if (*letter == '\n')
		{
			lib->cursorX = 0;
			lib->cursorY++;
		}

		if (lib->cursorX >= lib->windowWidth)
		{
			lib->cursorX = 0;
			lib->cursorY++;
			auto_newline=true;
		}

		if(*letter==' ')
		{
			letter++;
			goto count_word_len;
		}
		letter++;
	}
}

bool txtlib_deleteBackward(struct TextLib *lib)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->windowBg);

	// clear cursor
	txtlib_setCellAt(plane, lib->cursorX + lib->windowX, lib->cursorY + lib->windowY, lib->fontCharOffset, lib->charAttr);

	// move back cursor
	if (lib->cursorX > 0)
	{
		lib->cursorX--;
	}
	else if (lib->cursorY > 0)
	{
		lib->cursorX = lib->windowX + lib->windowWidth - 1;
		lib->cursorY--;
	}
	else
	{
		return false;
	}

	// clear cell
	txtlib_setCellAt(plane, lib->cursorX + lib->windowX, lib->cursorY + lib->windowY, lib->fontCharOffset, lib->charAttr);

	lib->core->interpreter->cycles += 4;
	return true;
}

void txtlib_writeText(struct TextLib *lib, const char *text, int x, int y)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->bg);
	const char *letter = text;
	while (*letter)
	{
		if (*letter >= 32)
		{
			char printableLetter = *letter;
			if (printableLetter >= 'a' && printableLetter <= 'z')
			{
				printableLetter -= 32;
			}
			txtlib_setCellAt(plane, x, y, lib->fontCharOffset + (printableLetter - 32), lib->charAttr);
			if (lib->windowBg != OVERLAY_BG)
			{
				lib->core->interpreter->cycles += 2;
			}
			x++;
		}
		letter++;
	}
}

void txtlib_writeNumber(struct TextLib *lib, int number, int digits, int x, int y)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->bg);

	if (number < 0)
	{
		// negative number
		number *= -1;
		txtlib_setCellAt(plane, x, y, lib->fontCharOffset + 13, lib->charAttr); // "-"
		x += digits;
		digits--;
	}
	else
	{
		x += digits;
	}

	int div = 1;
	for (int i = 0; i < digits; i++)
	{
		x--;
		txtlib_setCellAt(plane, x, y, lib->fontCharOffset + ((number / div) % 10 + 16), lib->charAttr);
		div *= 10;
	}

	if (lib->windowBg != OVERLAY_BG)
	{
		lib->core->interpreter->cycles += digits * 2;
	}
}

void txtlib_inputBegin(struct TextLib *lib)
{
	lib->inputBuffer[0] = 0;
	lib->inputLength = 0;
	lib->blink = 0;
	lib->core->machine->ioRegisters.key = 0;

	lib->core->machine->ioRegisters.status.keyboardVisible = 1;
	delegate_controlsDidChange(lib->core);

	txtlib_scrollWindowIfNeeded(lib);
}

bool txtlib_inputUpdate(struct TextLib *lib)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->windowBg);

	char key = lib->core->machine->ioRegisters.key;
	bool done = false;
	if (key)
	{
		if (key == CoreInputKeyBackspace)
		{
			if (lib->inputLength > 0)
			{
				if (txtlib_deleteBackward(lib))
				{
					lib->inputBuffer[--lib->inputLength] = 0;
				}
			}
		}
		else if (key == CoreInputKeyReturn)
		{
			// clear cursor
			txtlib_setCellAt(plane, lib->cursorX + lib->windowX, lib->cursorY + lib->windowY, lib->fontCharOffset, lib->charAttr);
			txtlib_printText(lib, "\n");
			done = true;
		}
		else if (key >= 32)
		{
			if (lib->inputLength < INPUT_BUFFER_SIZE - 2)
			{
				char text[2] = {key, 0};
				txtlib_printText(lib, text);
				lib->inputBuffer[lib->inputLength++] = key;
				lib->inputBuffer[lib->inputLength] = 0;

				txtlib_scrollWindowIfNeeded(lib);
			}
		}
		lib->blink = 0;
		lib->core->machine->ioRegisters.key = 0;
	}
	if (!done)
	{
		txtlib_setCellAt(plane, lib->cursorX + lib->windowX, lib->cursorY + lib->windowY, lib->fontCharOffset + (lib->blink++ < 30 ? 63 : 0), lib->charAttr);
		if (lib->blink == 60)
		{
			lib->blink = 0;
		}
	}
	return done;
}

void txtlib_clearWindow(struct TextLib *lib)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->windowBg);

	lib->cursorX = 0;
	lib->cursorY = 0;
	for (int y = 0; y < lib->windowHeight; y++)
	{
		int py = y + lib->windowY;
		for (int x = 0; x < lib->windowWidth; x++)
		{
			int px = x + lib->windowX;
			txtlib_setCellAt(plane, px, py, lib->fontCharOffset, lib->charAttr);
		}
	}
	lib->core->interpreter->cycles += lib->windowWidth * lib->windowHeight * 2;
}

void txtlib_clearScreen(struct TextLib *lib)
{
	struct VideoRegisters *reg = &lib->core->machine->videoRegisters;

	memset(&lib->core->machine->videoRam.planeA, 0, sizeof(struct Plane));
	memset(&lib->core->machine->videoRam.planeB, 0, sizeof(struct Plane));
	memset(&lib->core->machine->videoRam.planeC, 0, sizeof(struct Plane));
	memset(&lib->core->machine->videoRam.planeD, 0, sizeof(struct Plane));

	reg->scrollAX = 0;
	reg->scrollAY = 0;
	reg->scrollBX = 0;
	reg->scrollBY = 0;
	reg->scrollCX = 0;
	reg->scrollCY = 0;
	reg->scrollDX = 0;
	reg->scrollDY = 0;
	reg->attr.spritesEnabled = 1;
	reg->attr.planeAEnabled = 1;
	reg->attr.planeBEnabled = 1;
	reg->attr.planeCEnabled = 1;
	reg->attr.planeDEnabled = 1;

	lib->windowX = 0;
	lib->windowY = 0;
	lib->windowWidth = 27;
	lib->windowHeight = 48;
	lib->cursorX = 0;
	lib->cursorY = 0;
	lib->bg = 0;

	lib->core->interpreter->cycles += PLANE_COLUMNS * PLANE_ROWS * 2 * 2;
}

void txtlib_clearBackground(struct TextLib *lib, int bg)
{
	struct Plane *plane = txtlib_getBackground(lib, bg);
	memset(plane, 0, sizeof(struct Plane));
	lib->core->interpreter->cycles += PLANE_COLUMNS * PLANE_ROWS * 2;
}

struct Cell *txtlib_getCell(struct TextLib *lib, int x, int y)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->bg);
	return &plane->cells[y & ROWS_MASK][x & COLS_MASK];
}

void txtlib_setCell(struct TextLib *lib, int x, int y, int character)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->bg);
	txtlib_setCellAt(plane, x, y, character, lib->charAttr);
}

void txtlib_setCells(struct TextLib *lib, int fromX, int fromY, int toX, int toY, int character)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->bg);
	for (int y = fromY; y <= toY; y++)
	{
		for (int x = fromX; x <= toX; x++)
		{
			txtlib_setCellAt(plane, x, y, character, lib->charAttr);
		}
	}
	lib->core->interpreter->cycles += (toX - fromX + 1) * (toY - fromY + 1) * 2;
}

void txtlib_setCellsAttr(struct TextLib *lib, int fromX, int fromY, int toX, int toY, int pal, int flipX, int flipY, int prio)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->bg);
	for (int y = fromY; y <= toY; y++)
	{
		for (int x = fromX; x <= toX; x++)
		{
			struct Cell *cell = &plane->cells[y & ROWS_MASK][x & COLS_MASK];
			if (pal >= 0)
				cell->attr.palette = pal;
			if (flipX >= 0)
				cell->attr.flipX = flipX;
			if (flipY >= 0)
				cell->attr.flipY = flipY;
			if (prio >= 0)
				cell->attr.priority = prio;
		}
	}
	lib->core->interpreter->cycles += (toX - fromX + 1) * (toY - fromY + 1) * 2;
}

void txtlib_scrollBackground(struct TextLib *lib, int fromX, int fromY, int toX, int toY, int deltaX, int deltaY)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->bg);
	txtlib_scroll(plane, fromX, fromY, toX, toY, deltaX, deltaY);
	lib->core->interpreter->cycles += (toX - fromX + 1) * (toY - fromY + 1) * 2;
}

void txtlib_copyBackground(struct TextLib *lib, int srcX, int srcY, int width, int height, int dstX, int dstY)
{
	struct Plane *plane = txtlib_getBackground(lib, lib->bg);

	for (int y = 0; y < height; y++)
	{
		int py = dstY + y;
		int addr = lib->sourceAddress + ((srcY + y) * lib->sourceWidth + srcX) * 2;
		for (int x = 0; x < width; x++)
		{
			int px = dstX + x;
			struct Cell *cell = &plane->cells[py & ROWS_MASK][px & COLS_MASK];
			cell->character = machine_peek(lib->core, addr++);
			cell->attr.value = machine_peek(lib->core, addr++);
		}
	}
	lib->core->interpreter->cycles += width * height * 2;
}

int txtlib_getSourceCell(struct TextLib *lib, int x, int y, bool getAttrs)
{
	if (x >= 0 && y >= 0 && x < lib->sourceWidth && y < lib->sourceHeight)
	{
		int address = lib->sourceAddress + ((y * lib->sourceWidth) + x) * 2;
		if (getAttrs)
		{
			return machine_peek(lib->core, address + 1);
		}
		else
		{
			return machine_peek(lib->core, address);
		}
	}
	return -1;
}

bool txtlib_setSourceCell(struct TextLib *lib, int x, int y, int character)
{
	int address = lib->sourceAddress + ((y * lib->sourceWidth) + x) * 2;
	// only working RAM is allowed
	if (address < 0x9000 || address + 1 >= 0xE000)
	{
		return false;
	}

	if (character >= 0)
	{
		machine_poke(lib->core, address, character);
	}
	machine_poke(lib->core, address + 1, lib->charAttr.value);
	return true;
}

void txtlib_itobin(char *buffer, size_t buffersize, size_t width, int value)
{
	if (width < 1)
	{
		width = 1;
	}
	unsigned int mask = 1 << 15;
	int p = 0;
	bool active = false;
	while (mask && p < buffersize - 1)
	{
		if (active || (value & mask) || mask < (1 << width))
		{
			buffer[p++] = (value & mask) ? '1' : '0';
			active = true;
		}
		mask = (mask >> 1) & 0x7FFF;
	}
	buffer[p] = 0;
}
