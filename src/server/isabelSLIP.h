/*
   Isabel
   =========
   Copyright (C) 2016  Nelson Gonçalves
   
   License
   -------

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Summary
   -------

   Implementation of the SLIP encoding and decoding rules.

 */
#ifndef __ISABEL_SLIP_H__
#define __ISABEL_SLIP_H__

#include <QByteArray>


/*--------------------- Public Variable Declarations ----------------*/

/* SLIP encoding chars */
#define SLIP_END 		(0xC0)
#define SLIP_ESC 		(0xDB)
#define SLIP_ESC_END 	(0xDC)
#define SLIP_ESC_ESC 	(0xDD)

/*--------------------- Public Function Declarations ----------------*/

/* Encode a byte array using the SLIP protocol.

	@input the byte array to encode

	#returns the byte array as SLIP encoded
*/
QByteArray slip_encode(const QByteArray &input);

/* Decode a byte array using the SLIP protocol.

	@input the byte array to decode

	#returns the byte array after being slip decoded
*/
QByteArray slip_decode(const QByteArray &input);


#endif
