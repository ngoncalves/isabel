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

   Implementation of the Qt properties serialization. This is a very
   simple wrapper around the functionality provided by the qt-json
   code: https://github.com/gaudecker/qt-json

 */
#ifndef __ISABEL_SERIALIZE_H__
#define __ISABEL_SERIALIZE_H__

#include <QByteArray>
#include <QVariant>


/*--------------------- Public Variable Declarations ----------------*/

/*--------------------- Public Function Declarations ----------------*/

/* Encode the given QVariant onto a byte array

	@value the variant to encode

	#returns the QVariant encoded as a JSON string
*/
QByteArray serialize_encode(const QVariant& value);

/* Decode the variant from the given byte array

	@value JSON string from which to decode the QVariant

	#returns QVariant decoded
*/
QVariant serialize_decode(const QByteArray& value);

#endif
