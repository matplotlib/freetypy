/*
Copyright (c) 2015, Michael Droettboom
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
*/

#include "lcd.h"
#include "doc/lcd.h"

#include FT_LCD_FILTER_H

ftpy_ConstantType Py_FT_LCD_FILTER_ConstantType;
static PyTypeObject Py_FT_LCD_FILTER_Type;
#define LCD_FILTER_CONST(name) DEF_CONST(name, FT_LCD_FILTER)
static constant_def FT_LCD_FILTER_constants[] = {
    LCD_FILTER_CONST(NONE),
    LCD_FILTER_CONST(DEFAULT),
    LCD_FILTER_CONST(LIGHT),
    LCD_FILTER_CONST(LEGACY),
    {NULL}
};


int setup_Lcd(PyObject *m)
{
    return define_constant_namespace(
          m, &Py_FT_LCD_FILTER_Type, &Py_FT_LCD_FILTER_ConstantType,
          "freetypy.LCD_FILTER",
          doc_LCD_FILTER, FT_LCD_FILTER_constants);
}
