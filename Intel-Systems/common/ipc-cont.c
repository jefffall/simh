/*  ipc-cont.c: Intel IPC control port PIO adapter

    Copyright (c) 2010, William A. Beech

        Permission is hereby granted, free of charge, to any person obtaining a
        copy of this software and associated documentation files (the "Software"),
        to deal in the Software without restriction, including without limitation
        the rights to use, copy, modify, merge, publish, distribute, sublicense,
        and/or sell copies of the Software, and to permit persons to whom the
        Software is furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included in
        all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
        WILLIAM A. BEECH BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
        IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
        CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

        Except as contained in this notice, the name of William A. Beech shall not be
        used in advertising or otherwise to promote the sale, use or other dealings
        in this Software without prior written authorization from William A. Beech.

    MODIFICATIONS:

        07 Jun 16 - Original file.

    NOTES:


*/

#include "system_defs.h"                /* system header in system dir */

/* function prototypes */

uint8 ipc_cont(t_bool io, uint8 data, uint8 devnum);    /* ipc_cont*/
t_stat ipc_cont_reset (DEVICE *dptr, uint16 base, uint8 devnum);

/* external function prototypes */

extern uint8 reg_dev(uint8 (*routine)(t_bool, uint8, uint8), uint16 port, uint8 devnum);

/* globals */

UNIT ipc_cont_unit[] = {
    { UDATA (0, 0, 0) },                /* ipc_cont*/
};

REG ipc_cont_reg[] = {
    { HRDATA (CONTROL0, ipc_cont_unit[0].u3, 8) }, /* ipc_cont */
    { NULL }
};

DEBTAB ipc_cont_debug[] = {
    { "ALL", DEBUG_all },
    { "FLOW", DEBUG_flow },
    { "READ", DEBUG_read },
    { "WRITE", DEBUG_write },
    { "XACK", DEBUG_xack },
    { "LEV1", DEBUG_level1 },
    { "LEV2", DEBUG_level2 },
    { NULL }
};

/* address width is set to 16 bits to use devices in 8086/8088 implementations */

DEVICE ipc_cont_dev = {
    "IPC-CONT",             //name
    ipc_cont_unit,         //units
    ipc_cont_reg,          //registers
    NULL,               //modifiers
    1,                  //numunits
    16,                 //aradix
    16,                 //awidth
    1,                  //aincr
    16,                 //dradix
    8,                  //dwidth
    NULL,               //examine
    NULL,               //deposit
//    &ipc_cont_reset,       //reset
    NULL,       //reset
    NULL,               //boot
    NULL,               //attach
    NULL,               //detach
    NULL,               //ctxt
    0,                  //flags
    0,                  //dctrl
    ipc_cont_debug,        //debflags
    NULL,               //msize
    NULL                //lname
};

/*  I/O instruction handlers, called from the CPU module when an
    IN or OUT instruction is issued.
*/

/* IPC control port functions */

uint8 ipc_cont(t_bool io, uint8 data, uint8 devnum)
{
    if (io == 0) {                      /* read status port */
        sim_printf("   ipc_cont: read data=%02X port=%02X returned 0xFF\n", ipc_cont_unit[devnum].u3, devnum);
        return ipc_cont_unit[devnum].u3;
    } else {                            /* write control port */
        //this simulates an 74ls259 register 
        //d0-d2 address the reg(in reverse order!), d3 is the data to be latched
        switch(data & 0x07) {
            case 5:                     //interrupt enable
                if(data & 0x08)         //bit high
                    ipc_cont_unit[0].u3 |= 0x10;
                else                    //bit low
                    ipc_cont_unit[0].u3 &= 0xEF;
                break;
            case 4:                     //*selboot
                if(data & 0x08)         //bit high
                    ipc_cont_unit[0].u3 |= 0x08;
                else                    //bit low
                    ipc_cont_unit[0].u3 &= 0xF7;
                break;
            case 2:                     //*startup
                if(data & 0x08)         //bit high
                    ipc_cont_unit[0].u3 |= 0x04;
                else                    //bit low
                    ipc_cont_unit[0].u3 &= 0xFB;
                break;
            case 1:                     //over ride
                if(data & 0x08)         //bit high
                    ipc_cont_unit[0].u3 |= 0x02;
                else                    //bit low
                    ipc_cont_unit[0].u3 &= 0xFD;
                break;
            case 0:                     //aux prom enable
                if(data & 0x08)         //bit high
                    ipc_cont_unit[0].u3 |= 0x01;
                else                    //bit low
                    ipc_cont_unit[0].u3 &= 0xFE;
                break;
            default:
                break;
        }
        sim_printf("   ipc_cont: data=%02X ipc_cont[%d]=%02X\n", data, devnum, ipc_cont_unit[0].u3);
    }
    return 0;
}

/* Reset routine */

t_stat ipc_cont_reset(DEVICE *dptr, uint16 base, uint8 devnum)
{
    reg_dev(ipc_cont, base, devnum); 
    ipc_cont_unit[devnum].u3 = 0x00; /* ipc reset */
    sim_printf("   ipc_cont[%d]: Reset\n", devnum);
    sim_printf("   ipc_cont[%d]: Registered at %04X\n", devnum, base);
    return SCPE_OK;
}

/* end of ipc-cont.c */
